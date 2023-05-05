
#include<iostream>
#include"../log/log.hpp"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"lock_cond.hpp"
#include <queue>
#include<unistd.h>
using namespace std;
int startid = 1;



//生产
void* produce(void *arg){
    while(1){
        {
        //加锁==>构造时加锁，析构时自动解锁
        //生产
        std::cout<<"produce food val = "<<startid++<<std::endl;

        LOG_DEBUG("produce food val = "+std::to_string(startid-1));
        }
        //休息一下微妙
        usleep(800000);

    }
} 

//生产者与消费者模型
int main(){
    
    Log::getInstance()->initLog("./","mylog");
    //创建生产者

    pthread_t p1,p2;
    pthread_create(&p1, NULL, produce, NULL);
    pthread_detach(p1);
    pthread_create(&p2, NULL, produce, NULL);
    pthread_detach(p2);
   while(1){
        
    }    
    return 0;
}
