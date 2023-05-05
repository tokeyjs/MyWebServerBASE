#ifndef  LOG_H
#define LOG_H
#include<string>
#include <sys/types.h>
#include"../Time/mytime.hpp"
#include<sys/stat.h>
#include<fcntl.h>
#include<queue>
#include<unistd.h>
#include "../lock_cond/lock_cond.hpp"


//日志等级划分
enum Log_Level{
    DEBUG=1,
    INFO,
    WARN ,
    FATAL
};

//日志类  单例模式
class Log{
private:
     //构造函数
    Log()
         :fd_(-1)
         ,filename_("")
         ,maxLogs_(200)
         ,isClose_(false)
         ,isExit_(false)
    {}
    //析构函数
    ~Log(){
        isExit_ = true;
        isClose_ = true;
        MutexLockGuard loc(mutexlock_);
        while(!que_.empty()){
            cond_full.wait(mutexlock_.getMutex());
        }
        if(-1 != fd_)
            close(fd_); 
        delete instance;
    }
    

public:
    //初始化 (创建新线程专门写)
    void initLog(std::string dirName , std::string filename, int maxLog = 240, double fileMaxSize = 256, bool isClose = false);

    static Log* getInstance(); //获取一个实例
    //写日志到队列
    void writeLogToQue(std::string data, const char* file, int line, Log_Level level);

    //线程函数
    void flushLogs();
    
    static Log *instance;
    //适用于C接口的线程函数
    static void* func(void* arg);

    //检测文件大小，是否需要进行替换
    void replaceLogFile();

    void writeLogToQue_V(std::string &data,const char* file, int line, const char * level);
private:
   
    int fd_; //输出文件的文件描述符
    std::string filename_; //日志文件名称
    MutexLock mutexlock_; //保证日志队列的线成安全
    int maxLogs_;  //日志队列最大数量
    std::queue<std::string> que_;//日志队列
    bool isClose_; //日志功能是否关闭
    Cond cond_notfull; //条件变量
    Cond cond_full;
    bool isExit_; //日志退出==>通知写入线程将队列写入，等待全部写完再析构
    double fileMaxSize_; //一个日志文件的最大大小(mb)=达到是需要换文件
    std::string dirName_; //日志文件保存目录
};

#define LOG_DEBUG(str) Log::getInstance()->writeLogToQue(str,__FILE__,__LINE__, DEBUG);
#define LOG_INFO(str) Log::getInstance()->writeLogToQue(str, __FILE__,__LINE__, INFO);
#define LOG_WARN(str) Log::getInstance()->writeLogToQue(str, __FILE__,__LINE__, WARN);
#define LOG_FATAL(str) Log::getInstance()->writeLogToQue(str, __FILE__,__LINE__, FATAL);

#endif
                         
