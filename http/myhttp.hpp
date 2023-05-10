#ifndef MYHTTP_H
#define MYHTTP_H

#include<stdio.h>
#include<sys/socket.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string>
#include<string.h>
#include"../ThreadPool/mytask.hpp"
#include<sys/types.h>
#include<sys/socket.h>
using namespace std;

//解析http请求
class MyHttp:public MyTask{
public:

    MyHttp(int socketfd = -1, int cfd = -1, string request = "")
        :request_(request)
        ,socketFd_(socketfd)
        ,cfd_(cfd)
    {
        //解析请求
        analyseHttp();

        //分析客户端信息
        analyseCilent();
    }

    ~MyHttp(){}

    //处理request请求
    virtual Any run(){
    
        std::cout<<cilentIp_<<":"<<cilentPort_<<" "<<method_<<" "<<url_<<" "<<protocol_<<std::endl;

        //给客户端回复
        //send(cfd_, "hello",6,0);
        if(0 == strcmp("GET", method_.c_str())){
            dealGetRequest();
                            
        }else if(0 == strcmp("POST", method_.c_str())){
            dealPostRequest();

        }else{
            dealOtherRequest();

        }
        return Any();
    }


private:
    //处理GET请求
    void dealGetRequest(){
    
    
    }

    //处理POST请求
    void dealPostRequest(){
    
    } 
    //处理其他请求
    void dealOtherRequest(){
    
    
    } 

    //发送http头部包
    void sendHttpHead(){
    
    }

    //0拷贝发送文件
    void sendFile(string filename){
    
    
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

    string request_; //请求内容
    string method_; //请求方法
    string  protocol_; //协议版本
    string url_; //请求路由
    string data_; //请求具体内容数据
    int socketFd_; //服务端fd
    int cfd_; //客户端fd
    string cilentIp_; //客户端ip
    int cilentPort_;  //客户端port
};



#endif
