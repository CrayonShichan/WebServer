#include "locker.h"

// 互斥量类
locker::locker() {
    if(pthread_mutex_init(&m_mutex, nullptr)) {
        throw std::exception();
    }
}

locker::~locker() {
    pthread_mutex_destroy(&m_mutex);
} 

bool locker::lock() {
    return pthread_mutex_lock(&m_mutex) == 0;
}

bool locker::unlock() {
    return pthread_mutex_unlock(&m_mutex) == 0;
}

pthread_mutex_t* locker::get() {
    return &m_mutex;
}


// 条件变量类
cond::cond() {
    if(pthread_cond_init(&m_cond, nullptr)) {
        throw std::exception();
    }
}

cond::~cond() {
    pthread_cond_destroy(&m_cond);
}

bool cond::wait(pthread_mutex_t *m_mutex) {
    return pthread_cond_wait(&m_cond, m_mutex);
}

bool cond::timewait(pthread_mutex_t *m_mutex, struct timespec t) {
    return pthread_cond_timedwait(&m_cond, m_mutex, &t);
}

bool cond::signal() {
    return pthread_cond_signal(&m_cond) == 0;
}

bool cond::broadcast() {
    return pthread_cond_broadcast(&m_cond) == 0;
}


// 信号量类
sem::sem(int num) {
    if(sem_init(&m_sem, 0, num)) {
        throw std::exception();
    }
}

sem::~sem() {
    sem_destroy(&m_sem);
}

bool sem::wait() {
    return sem_wait(&m_sem) == 0;
}

bool sem::post() {
    return sem_post(&m_sem) == 0;
}
