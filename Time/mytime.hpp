#ifndef MYTIME_H
#define MYTIME_H
#include<sys/time.h>
#include<ctime>
#include<string.h>
#include<time.h>
#include<string>

//封装时间类
class MyTime{
public:
    MyTime() = default;
    ~MyTime() = default;
    //获取时间戳精确到秒
    static time_t getTimeStamp_sec(){
        struct timeval val;
        gettimeofday(&val, NULL);
        return val.tv_sec; 
    }
      //获取字符串时间戳精确到秒
    static std::string getTimeStampString_sec(){
        struct timeval val;
        gettimeofday(&val, NULL);
        return std::to_string(val.tv_sec); 
    }
    //获取字符串时间戳精确到毫秒
    static std::string getTimeStampString_msec(){
        struct timeval val;
        gettimeofday(&val, NULL);
        return std::to_string(val.tv_sec)+std::to_string(val.tv_usec/1000); 
    }
     //获取字符串时间戳精确到微秒
    static std::string getTimeStampString_usec(){
        struct timeval val;
        gettimeofday(&val, NULL);
        return std::to_string(val.tv_sec)+std::to_string(val.tv_usec); 
    } 

    //获取字符串式的时间 ==>Wed Apr 26 20:23:58 2023
    static std::string getTimeString(){
        time_t t;
        t = time(NULL);
        std::string s = ctime(&t);
        s.erase(s.size()-1,1);
        return std::move(s);
    }    
};



#endif
