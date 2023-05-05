#include<iostream>
#include"../log/log.hpp"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"lock_cond.hpp"
#include <queue>
#include<unistd.h>
using namespace std;
queue<int> que;
int startid = 1;
const int MAXSIZE = 10;

MutexLock mutexlock;

Cond conda;

//生产
void* produce(void *arg){
    while(1){
        {
        //加锁==>构造时加锁，析构时自动解锁
        MutexLockGuard loc(mutexlock);
        while(que.size()==MAXSIZE){
            conda.wait(mutexlock.getMutex());
        }
        //生产
        que.push(startid++);
        std::cout<<"produce food val = "<<startid-1<<std::endl;

        LOG_DEBUG("produce food val = "+std::to_string(startid-1));
        conda.notifyAll();
        }
        //休息一下微妙
        usleep(800000);

    }
} 

//消费
void* consume(void *arg){
    while(1){
          //加锁
         MutexLockGuard loc(mutexlock);
         while(que.size()==0){
             conda.wait(mutexlock.getMutex());
         }
         //消费
         int val = que.front();
         std::cout<<"consume food val = "<<val<<std::endl;
         que.pop();
         LOG_DEBUG("consume food val = "+std::to_string(val));
         //唤醒因条件变量阻塞的线程
         conda.notifyAll();
    }

}

//生产者与消费者模型
int main(){
    
    Log::getInstance()->initLog("./mylog.txt");
    //创建生产者

    pthread_t p1,p2;
    pthread_create(&p1, NULL, produce, NULL);
    pthread_detach(p1);
    pthread_create(&p2, NULL, produce, NULL);
    pthread_detach(p2);
    //创建消费者
    pthread_t c1,c2,c3;
    pthread_create(&c1, NULL, consume, NULL);
    pthread_detach(c1);
    pthread_create(&c2, NULL, consume, NULL);
    pthread_detach(c2);   
    pthread_create(&c3, NULL, consume, NULL);
    pthread_detach(c3);  
    while(1){
        
    }    
    return 0;
}
