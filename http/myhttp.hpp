#ifndef MYHTTP_H
#define MYHTTP_H

#include<stdio.h>
#include<string>
#include"../ThreadPool/mytask.hpp"
#include<sys/types.h>
#include<sys/socket.h>
using namespace std;

//解析http请求
class MyHttp:public MyTask{
public:

    MyHttp(int cfd = -1, string request = "")
        :request_(request)
        ,cfd_(cfd)
    {
        analyseHttp();
    }

    ~MyHttp(){}

    Any run(){
    
        std::cout<<method_<<" "<<url_<<" "<<protocol_<<std::endl;

        //给客户端回复
        send(cfd_, "hello",6,0);


        return Any();
    }
private:
    
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
    int cfd_; //客户端fd
};



#endif
