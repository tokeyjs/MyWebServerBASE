#ifndef MYHTTP_H
#define MYHTTP_H

#include <cerrno>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<string.h>
#include"../ThreadPool/mytask.hpp"
#include"../log/log.hpp"
#include<sys/types.h>
#include<sys/socket.h>
using namespace std;

//解析http请求
class MyHttp:public MyTask{
public:

    MyHttp(int epfd = -1, int cfd = -1, string request = "")
        :request_(request)
        ,epFd_(epfd)
        ,cfd_(cfd)
    {
        dirname_ = "./data";
        //解析请求
        analyseHttp();

        //分析客户端信息
        analyseCilent();
    }

    ~MyHttp(){}

    //处理request请求
    virtual Any run(){
    
        //std::cout<<cilentIp_<<":"<<cilentPort_<<" "<<method_<<" "<<url_<<" "<<protocol_<<std::endl;

        //给客户端回复
        //send(cfd_, "hello",6,0);
        if(0 == strcmp("GET", method_.c_str())){
            dealGetRequest();
                            
        }else if(0 == strcmp("POST", method_.c_str())){
            dealPostRequest();

        }else{
            dealOtherRequest();

        }
        
        //关闭连接
        int ret = epoll_ctl(epFd_, EPOLL_CTL_DEL, cfd_, NULL);
        close(cfd_);
        //std::cout<<cfd_<<" exit..."<<std::endl;
        /*
        struct epoll_event ev ;
        ev.data.fd = cfd_;
        ev.events = EPOLLHUP|EPOLLERR|EPOLLIN|EPOLLET| EPOLLONESHOT;
        epoll_ctl(epFd_, EPOLL_CTL_MOD, cfd_, &ev);    
        */
        return Any();
    }


private:
    //处理GET请求
    void dealGetRequest(){
      string path = dirname_+url_;     
      struct stat buf;
      int ret = stat( path.c_str(), &buf);
      if(-1 == ret){
        if(errno == ENOENT){
          //文件不存在
          //发送error.html
          path = dirname_ + "/error.html";
          //发头部
          sendHttpHead(404, "Not Found", get_mime_type(const_cast<char*>(path.c_str())), getFileSize(path));
          sendFile(path);
          
          LOG_WARN("no such file");
          return;
        }  
        perror("stat");
      }
      
      //是目录
      if(buf.st_mode & S_IFDIR){
          path = dirname_ + "/error.html";
          
          sendHttpHead(404, "Not Found", get_mime_type(const_cast<char*>(path.c_str())),getFileSize(path));
          sendFile(path);

        LOG_WARN("request is dir");    

      }else if(buf.st_mode & S_IFREG){
          //常规文件
          sendHttpHead(200, "OK", get_mime_type(const_cast<char*>(path.c_str())),getFileSize(path));
          sendFile(path);
      }else {
          path = dirname_ + "/error.html";
          sendHttpHead(404, "Not Found", get_mime_type(const_cast<char*>(path.c_str())),getFileSize(path));
          sendFile(path); 

          LOG_WARN("request noreg file");
      }
    
    }

    //处理POST请求
    void dealPostRequest(){
    
    } 
    //处理其他请求
    void dealOtherRequest(){
    
    
    } 

    //发送http头部包
    void sendHttpHead(int code, const char* info, const char *contenttype, int contentLen, bool sendLenth = true){
      char buf[512];
      int len_;
      //这个长度有时横重要==不写浏览器就会一直加载除非closefd
      if(sendLenth){
        len_ = sprintf(buf, "HTTP/1.1 %d %s\r\nContent-Type:%s\r\nContent-Length:%d\r\n\r\n"
                         ,code, info, contenttype, contentLen);
      }else{
        len_ = sprintf(buf, "HTTP/1.1 %d %s\r\nContent-Type:%s\r\n\r\n"
                         ,code, info, contenttype);
      }
      
      send(cfd_, buf, len_, 0);
    }

    //0拷贝发送文件
    void sendFile(string filename){
      int fd = open(filename.c_str(), O_RDWR); 
      if(-1 == fd){
        perror("open");
        return;
      }
      struct stat sbuf;
      stat(filename.c_str(), &sbuf);
      sendfile(cfd_, fd, NULL, sbuf.st_size);    
      close(fd);
    }

      //分析客户端信息
    void analyseCilent(){
        struct sockaddr_in addr;
        socklen_t slen = sizeof(addr);
        getpeername(cfd_, (struct sockaddr*)&addr, &slen);

        char ip[16]{0};
        cilentPort_ = ntohs(addr.sin_port);
        inet_ntop(AF_INET,&addr.sin_addr,ip,16);
    
        cilentIp_ = ip;
    }


    //返回文件类型对应的contenttype字段
    // 处理contenttype
    const char *get_mime_type(char *name){
        char *dot;
        dot = strrchr(name, '.');
        if (dot == (char *)0)
            return "text/plain; charset=utf-8";
        if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
            return "text/html; charset=utf-8";
        if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
            return "image/jpeg";
        if (strcmp(dot, ".gif") == 0)
            return "image/gif";
        if (strcmp(dot, ".png") == 0)
            return "image/png";
        if (strcmp(dot, ".css") == 0)
            return "text/css";
        if (strcmp(dot, ".au") == 0)
            return "audio/basic";
        if (strcmp(dot, ".wav") == 0)
            return "audio/wav";
        if (strcmp(dot, ".avi") == 0)
            return "video/x-msvideo";
        if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
            return "video/quicktime";
        if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
            return "video/mpeg";
        if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
            return "model/vrml";
        if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
            return "audio/midi";
        if (strcmp(dot, ".mp3") == 0)
            return "audio/mpeg";
        if (strcmp(dot, ".ogg") == 0)
            return "application/ogg";
        if (strcmp(dot, ".pac") == 0)
            return "application/x-ns-proxy-autoconfig";

        return "text/plain; charset=utf-8";
    }


    //解析请求
    void analyseHttp(){
       char m[10]{0};
       char url[256]{0};
       char p[24]{0};

       sscanf(request_.c_str(), "%[^ ] %[^ ] %[^\r]",m,url,p);
       method_ = m;
       url_ = url;
       protocol_ = p;
    }
    
    //求文件大小
    int getFileSize(string filename){
      struct stat sbuf;
      stat(filename.c_str(), &sbuf);
      return sbuf.st_size;
    }

    string request_;//请求内容
    string dirname_; //资源目录
    string method_; //请求方法
    string  protocol_; //协议版本
    string url_; //请求路由
    string data_; //请求具体内容数据
    int epFd_; //服务端fd
    int cfd_; //客户端fd
    string cilentIp_; //客户端ip
    int cilentPort_;  //客户端port
};



#endif
