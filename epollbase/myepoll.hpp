
#ifndef MYEPOLL_H
#define MYEPOLL_H

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
#include<atomic> //记录epollfd上的事件数


const int maxEventSize = 4096;


class MyEpoll{
public:
    MyEpoll()
        :eventSize_(0)
         ,epfd_(-1)
    {
        epfd_ = epoll_create(1);

    
    }
    ~MyEpoll(){
    
    
    
    }
    //wait
    void listen_wait(){
        while(1){
            //等待监听事件的改变
            int narr = epoll_wait(epfd_, evArray_, maxEventSize, -1);
            for(int i=0; i<narr; i++){
                  //!!!!!!!!!!!!!!!!!!!!!!!!!!!
            }

        }
    
    }
    
    //epoll上注册事件
    int addEvent(int cfd, int events = EPOLLIN){
        struct epoll_event ev ;
        ev.data.fd = cfd;
        ev.events = events;
        eventSize_++;
        return epoll_ctl(epfd_, EPOLL_CTL_ADD, cfd, &ev);

    }
 
    //epoll上注销事件
    int delEvent(int cfd){

        int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, cfd, NULL);
        close(cfd);
        eventSize_--;
        return ret;
    }   
    //加上EPOLLONESHOT
    int addOneShot(int cfd, int events = EPOLLIN){
        struct epoll_event ev ;
        ev.data.fd = cfd;
        ev.events = events | EPOLLONESHOT;
        return epoll_ctl(epfd_, EPOLL_CTL_MOD, cfd, &ev);  
    }
      //去除EPOLLONESHOT
    int delOneShot(int cfd, int events = EPOLLIN){
        struct epoll_event ev ;
        ev.data.fd = cfd;
        ev.events = (events&(~EPOLLONESHOT));
        return epoll_ctl(epfd_, EPOLL_CTL_MOD, cfd, &ev);  
    }
     

private:
    std::atomic_int eventSize_; //当前事件数量
    int epfd_; //
    struct epoll_event evArray_[maxEventSize]; //发生变化的

};
















const int epollSize = 2;
int EpollArray[epollSize];

int ptrEpoll = 0;

//epoll上注册事件
int myEpolladd(int epfd, int cfd, int events){
    struct epoll_event ev ;
    ev.data.fd = cfd;
    ev.events = events;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);

}
 
//epoll上注销事件
int myEpolldel(int epfd, int cfd){

    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
    close(cfd);
    return ret;
}   


void* ThreadFunc(void* arg){
//线程函数==》epoll线程
    int *epollfd = (int*)arg;

    *epollfd = epoll_create(1);

    struct epoll_event ep_ev_arr[2048];
    int narray;

    while(1){
        narray = epoll_wait(*epollfd, ep_ev_arr,2048, -1);
        for(int i=0;i<narray;i++){
            char buf[1024];
            bzero(buf, 1024);
            int ret = read(ep_ev_arr[i].data.fd, buf, 1024);
            if(ret == 0){
                //
                myEpolldel(*epollfd, ep_ev_arr[i].data.fd);

                std::cout<<"exit "<<  ep_ev_arr[i].data.fd<<std::endl;
                continue;
            }else if(ret == -1){
                perror("epoll_read");
                continue;
            }else{
                write(ep_ev_arr[i].data.fd, buf, strlen(buf));
            }
        }
    
    }



    return NULL;
}



int main(){

    //创建套接子
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
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

    //新建多个epoll线程
    for(int i=0;i<epollSize;i++){
        pthread_t p;
        pthread_create(&p, NULL, ThreadFunc, &EpollArray[i]);
        pthread_detach(p);
    }

    while(1){
        //接受新客户端连接
        int cfd = accept(sfd, NULL, NULL);
        std::cout<<"new "<<cfd<<std::endl;
        // write(cfd, "hello\n", 6);
        // 负载均衡函数
        ptrEpoll = (ptrEpoll+1)%epollSize;

        //给EpollArray[ptrEpoll]上注册cfd
        myEpolladd(EpollArray[ptrEpoll++], cfd, EPOLLIN);

    }


    return 0;
}


#endif
