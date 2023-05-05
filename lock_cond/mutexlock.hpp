#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include<pthread.h>

//封装mutex
class MutexLock{
public:
	MutexLock(){
		pthread_mutex_init(&mutex_, NULL);
	}
    ~MutexLock(){
        pthread_mutex_destroy(&mutex_);
    }
    void lock(){
        pthread_mutex_lock(&mutex_);
    }
    //解锁
    void unlock(){
        pthread_mutex_unlock(&mutex_); 
    }
    //获取互斥锁
    pthread_mutex_t& getMutex(){
        return mutex_;
    }
private:
	pthread_mutex_t mutex_;
};                         


#endif
