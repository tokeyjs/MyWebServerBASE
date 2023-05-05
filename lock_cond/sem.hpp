#ifndef SEM_H
#define SEM_H
#include"mutexlock.hpp"
#include"cond.hpp"
#include"mutexlockguard.hpp"

//实现信号量
class Sem{
public:
    Sem(int limit=0)
        :limit_(limit)
    {}

    //释放一个资源
    void post(){
        MutexLockGuard loc(mutex_);
        limit_++;

        cond_.notifyAll();
    }

    //申请一个资源
    void wait(){
        MutexLockGuard loc(mutex_);
        while(limit_<=0){
            cond_.wait(mutex_.getMutex());
        }

        limit_--;
    }

private:
    MutexLock mutex_;
    Cond cond_;
    unsigned short int limit_; //资源数
};

#endif
