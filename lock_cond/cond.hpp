#ifndef COND_H
#define  COND_H
#include<time.h>
#include<sys/time.h>
#include <pthread.h>


//条件变量封装

class Cond{
public: 
    Cond(){
            pthread_cond_init(&cond_,NULL);

    }
    ~Cond(){
            pthread_cond_destroy(&cond_);
    }

    void wait(pthread_mutex_t& mutex){
        pthread_cond_wait(&cond_, &mutex);
    }

    int notify(){
        return pthread_cond_signal(&cond_);
    }

    int notifyAll(){
        return pthread_cond_broadcast(&cond_);
    }
    //超时等待 传入超时参数 毫秒为单位
    int timeWait(pthread_mutex_t& mutex, int msec){
        struct timeval val;
        gettimeofday(&val, NULL);
        struct timespec abstime;
        //获取时间
        abstime.tv_sec = val.tv_sec + (msec/1000);
        abstime.tv_nsec =val.tv_usec*1000 + (1000*1000*(msec%1000));
        return pthread_cond_timedwait(&cond_, &mutex, &abstime);
    }    

    pthread_cond_t cond_;
};


#endif
