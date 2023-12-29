#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

// 互斥锁类
class locker {
private:
    pthread_mutex_t m_mutex;
public:
    locker();
    ~locker();

    bool lock();

    bool unlock();

    pthread_mutex_t *get();
};

// 条件变量类
class cond {
private:
    pthread_cond_t m_cond;
public:
    cond();
    ~cond();

    bool wait(pthread_mutex_t *mutex);

    bool timewait(pthread_mutex_t *mutex, struct timespec t);

    bool signal();

    bool broadcast();
};


// 信号量类
class sem {
private:
    sem_t m_sem;
public:
    sem(int num = 0);
    ~sem();

    bool wait();
    bool post();

};

#endif
