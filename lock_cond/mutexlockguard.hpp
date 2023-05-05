
#ifndef MUTEXLOCKGUARD_H
#define MUTEXLOCKGUARD_H

#include<pthread.h>
#include"mutexlock.hpp"

//封装mutexlock
class MutexLockGuard{
public:
    MutexLockGuard(MutexLock &mutex)
        :mutexlock_(mutex)
    {
        mutexlock_.lock();        
    } 
    ~MutexLockGuard(){
        mutexlock_.unlock();        
    }
   pthread_mutex_t& getMutex(){
    return mutexlock_.getMutex();
   } 
private:
    MutexLock& mutexlock_;

};

#endif
