
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
#include"../ThreadPool/threadpool.hpp"
#include"../http/myhttp.hpp"
#include<fcntl.h>


const int maxEventSize = 10240;

class MyEpoll{
public:
    MyEpoll(int socketFd = -1)
        :socketFd_(socketFd)
        ,eventSize_(0)
         ,epfd_(-1)
    {
        epfd_ = epoll_create(1);
        pool_.setModel(CACHE); //设置动态增长线程
    }
    ~MyEpoll(){
   
    }
    //wait
    void listen_wait(){
        while(1){
            //等待监听事件的改变
            int narr = epoll_wait(epfd_, evArray_, maxEventSize, -1);
            for(int i=0; i<narr; i++){
                if(evArray_[i].events & EPOLLIN){
                    //读事件
                    char buff[3096];
                    bzero(buff, 3096);
                    int ret = recv(evArray_[i].data.fd, buff, 3096, 0);
                    if(0 > ret){
                        if(errno == EAGAIN||errno==EWOULDBLOCK){
                            //出现在非阻塞模式
                            //LOG_WARN("recv EAGAN|EWOULDBLOCK");
                            continue;
                        }
                        perror("recv");
                    }else if(0 == ret){
                        //客户端退出
                        delEvent(evArray_[i].data.fd);

                    }else{
                        //向线程池提交任务
                        std::shared_ptr<MyHttp> taskptr(new MyHttp(epfd_, evArray_[i].data.fd, buff));
                        pool_.commitTask(taskptr);
                        
                        eventSize_--;
                    }

                }else if(evArray_[i].events & EPOLLERR){
                    //发生错误
                    delEvent(evArray_[i].data.fd);
                    //LOG_WARN(" EPOLLERR");

                }else if(evArray_[i].events & EPOLLHUP){
                    //客户端断开
                    //LOG_INFO(" EPOLLHUP");
                    delEvent(evArray_[i].data.fd);

                }else{
                    //其他...
                    //LOG_WARN("Other Event");
                    delEvent(evArray_[i].data.fd);
                }

            }

        }
    
    }
    
    //epoll上注册事件
    int addEvent(int cfd, int events = EPOLLIN|EPOLLERR|EPOLLHUP){
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
        //std::cout<<cfd<<" exit..."<<std::endl;
        eventSize_--;
        return ret;
    }   
    //加上EPOLLONESHOT
    int addOneShot(int cfd, int events = EPOLLIN|EPOLLERR|EPOLLHUP){
        struct epoll_event ev ;
        ev.data.fd = cfd;
        ev.events = events | EPOLLONESHOT;
        return epoll_ctl(epfd_, EPOLL_CTL_MOD, cfd, &ev);  
    }
      //去除EPOLLONESHOT
    int delOneShot(int cfd, int events = EPOLLIN|EPOLLERR|EPOLLHUP){
        struct epoll_event ev ;
        ev.data.fd = cfd;
        ev.events = (events&(~EPOLLONESHOT));
        return epoll_ctl(epfd_, EPOLL_CTL_MOD, cfd, &ev);  
    }
     

private:
    int socketFd_; //服务端socketfd
    std::atomic_int eventSize_; //当前事件数量
    int epfd_; //
    struct epoll_event evArray_[maxEventSize]; //发生变化的
    ThreadPool pool_; //工作线程池

};




#endif
