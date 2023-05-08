#ifndef MYHTTP_H
#define MYHTTP_H

#include<string>
#include"../ThreadPool/mytask.hpp"
using namespace std;

//解析http请求
class MyHttp:public MyTask{
public:

    MyHttp(string request = "")
        :request_(request)
    {
    
    }

    Any run(){
    
        
        return Any();
    }

private:
    //分析请求
    void analyseHttp(){
       
    }

    string request_; //请求内容
    string method_; //请求方法
    string url_; //请求路由
    string data_; //请求具体内容数据
};



#endif
