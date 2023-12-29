#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include "./noactive/lst_timer.h"
#include "./http/http_conn.h"
#include "./threadpool/threadpool.h"
#include "./lock/locker.h"

#define MAX_FD 65536   // 最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000  // 监听的最大的事件数量
#define TIMESLOT 5

// 互斥锁
locker work_locker;

// 接受sigalrm信号
static int pipefd[2];
static sort_timer_lst timer_lst;

extern int setNonBlocking(int fd);
// 添加文件描述符
extern void addfd( int epollfd, int fd, bool one_shot, bool flag = false);
extern void removefd( int epollfd, int fd );

void addsig(int sig, void(handler)(int)) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    // 注册信号捕捉函数
    assert(sigaction(sig, &sa, NULL) != -1);
}

void sig_handler( int sig )
{
    // printf("%d\n", sig);
    int save_errno = errno;
    int msg = sig;
    send( pipefd[1], ( char* )&sig, 1, 0);
    errno = save_errno;
}

// 定时器回调函数，它删除非活动连接socket上的注册事件，并关闭之。
void cb_func( http_conn *user_conn) {
    // 加锁， 防止正在读或者写
    work_locker.lock();
    user_conn->close_conn();
    work_locker.unlock();
}

int main(int argc, char* argv[]) {
    if( argc <= 1 ) {
        printf( "usage: %s port_number\n", basename(argv[0]));
        return 1;
    }

    int port = atoi( argv[1]);
    addsig( SIGPIPE, SIG_IGN );

    threadpool< http_conn >* pool = nullptr;
    try {
        pool = new threadpool<http_conn>();
    } catch( ... ) {
        return -1;
    }

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    int ret = 0;
    struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // 端口复用
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定
    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    ret = listen( listenfd, 5 );

    // 创建epoll对象，和事件数组，添加
    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 5 );

    // 添加到epoll对象中
    addfd( epollfd, listenfd, false, true);
    http_conn::m_epollfd = epollfd;

    // 创建管道
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setNonBlocking(pipefd[1]);

    addfd( epollfd, pipefd[0], false);

    // 设置信号处理函数
    addsig( SIGALRM, sig_handler);
    addsig( SIGTERM, sig_handler);

    client_data* users = new client_data[MAX_FD]; 
    bool timeout = false;

    struct itimerval new_value;

    // 设置间隔的时间, 时间间隔为5s
    // new_value.it_interval.tv_sec = TIMESLOT;
    // new_value.it_interval.tv_usec = 0;

    // // 设置延迟的时间,0秒之后开始第一次定时
    // new_value.it_value.tv_sec = TIMESLOT;
    // new_value.it_value.tv_usec = 0;
    
    // ret = setitimer(ITIMER_REAL, &new_value, NULL); // 非阻塞的  定时,5秒后产生SIGALARM信号
    alarm(TIMESLOT);
    assert(ret != -1);

    bool stop_server = false;

    while(!stop_server) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ( ( number < 0 ) && ( errno != EINTR ) ) {
            printf( "epoll failure\n" );
            break;
        }
        for(int i = 0; i < number; ++ i) {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd) {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                
                if ( connfd < 0 ) {
                    printf( "errno is: %d\n", errno );
                    continue;
                } 

                if( http_conn::m_user_count >= MAX_FD ) {
                    close(connfd);
                    continue;
                }
                http_conn http_user;
                http_user.init(connfd, client_address);
                users[connfd].user_conn = &http_user;
                
                // 创建定时器，设置其回调函数与超时时间，然后绑定定时器与用户数据，最后将定时器添加到链表timer_lst中
                util_timer* timer = new util_timer;
                timer->user_data = &users[connfd];
                timer->cb_func = cb_func;
                time_t cur = time( NULL );
                timer->expire = cur + 3 * TIMESLOT;
                users[connfd].timer = timer;
                timer_lst.add_timer( timer );
            } else if(( sockfd == pipefd[0] ) && ( events[i].events & EPOLLIN ) ) {
                // 处理信号
                int sig;
                char signals[MAX_FD];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                if(ret == -1 || ret == 0)  {
                   continue;
                }  else {
                    for(int i = 0; i < ret; ++ i) {
                        // printf("process signal\n");
                        switch( signals[i] )  {
                            case SIGALRM:
                            {
                                // 用timeout变量标记有定时任务需要处理，但不立即处理定时任务
                                // 这是因为定时任务的优先级不是很高，我们优先处理其他更重要的任务。
                                timeout = true;
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = true;
                            }
                        }
                    }
                }
            } else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {

                users[sockfd].user_conn->close_conn();
                timer_lst.del_timer(users[sockfd].timer);

            } else if(events[i].events & EPOLLIN) {

                if(users[sockfd].user_conn->read()) {
                    pool->append(users[sockfd].user_conn);
                } else {
                    users[sockfd].user_conn->close_conn();
                    // 删除该定时器
                    timer_lst.del_timer(users[sockfd].timer);
                }

            } else if( events[i].events & EPOLLOUT ) {

                if( !users[sockfd].user_conn->write() ) {
                    users[sockfd].user_conn->close_conn();
                    timer_lst.del_timer(users[sockfd].timer);
                }

            }
        }

        // 最后处理定时事件，因为I/O事件有更高的优先级。当然，这样做将导致定时任务不能精准的按照预定的时间执行。
        if( timeout ) {
            // 定时处理任务，实际上就是调用tick()函数
            timer_lst.tick();
            alarm(TIMESLOT);
            timeout = false;
        }
    }

    close( listenfd );
    close( pipefd[1] );
    close( pipefd[0] );
    delete [] users;
    delete pool;
    return 0;
}