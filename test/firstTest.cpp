
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<iostream>
#include<pthread.h>
#include"../epollbase/myepoll.hpp"

MyEpoll* EpollThreadArr[2];
int ptrEpoll = 0;
int epollSize = 2;
int sfd = -1; //socketfd

//创建epollfd的线程函数 arg==> 你给一个索引
void* epollFdThreadFuc(void *arg){
    //创建两个epollfd
    MyEpoll** p = static_cast<MyEpoll**>(arg);
    *p =  new MyEpoll(sfd);

    std::cout<<"epollThread start..."<<std::endl;
    (*p)->listen_wait();
     
    return NULL;
}



int main(){

    //创建套接子
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sfd){
        perror("socket ");
        return 1;
    }
        

    //绑定
    struct sockaddr_in addr;
    socklen_t tlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8999);
    inet_pton(AF_INET, "127.0.0.1", (struct sockaddr*)&addr.sin_addr.s_addr);

    int ret = bind(sfd, (struct sockaddr*)&addr, tlen);
    if(-1 == ret){
        perror("bind");
        return 1;
    }

    //监听
    listen(sfd, 128);


    //创建
    for(int i=0;i<epollSize;i++){
        pthread_t tt;
        pthread_create(&tt, NULL, epollFdThreadFuc, (void*)&EpollThreadArr[i]);
        pthread_detach(tt);
    }

    
    while(1){
        //接受新客户端连接
        int cfd = accept(sfd, NULL, NULL);
        std::cout<<"new "<<cfd<<std::endl;
        // write(cfd, "hello\n", 6);
        // 负载均衡函数
        ptrEpoll = (ptrEpoll+1)%epollSize;

        //给EpollArray[ptrEpoll]上注册cfd
        EpollThreadArr[ptrEpoll]->addEvent(cfd);
    }








    return 0;
}
