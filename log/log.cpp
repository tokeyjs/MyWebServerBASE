#include"log.hpp"
#include<sys/time.h>
#include<time.h>
#include<functional>
#include<assert.h>
#include<pthread.h>
#include<iostream>
#include<string.h>
//绑定的线程函数
void* Log::func(void* arg){
    Log *p = (Log*)arg;
    p->flushLogs();
    return NULL;
}

Log* Log::instance = new Log();

Log* Log::getInstance(){
    return instance;
}               
//初始化 (创建新线程专门写)

void Log::initLog(std::string dirName, std::string filename, int maxLog, double fileMaxSize ,bool isClose){
    
    filename_ = filename;
    maxLogs_ = maxLog;
    isClose_ = isClose;
    dirName_ = dirName;
    fileMaxSize_ = fileMaxSize;
     //需要新创建文件
     char newFileName[125] = {0};
     sprintf(newFileName,"%s%s_%s_.log",dirName_.c_str() , filename_.c_str() , MyTime::getTimeStampString_sec().c_str()); 
    //打开文件获取文件描述符
    fd_ = open(newFileName, O_RDWR|O_APPEND|O_CREAT, 0666);
    assert(fd_!=-1);
    //创建线程
    pthread_t p1;
    pthread_create(&p1, NULL,func,(void*)this);
    pthread_detach(p1);

}

 //写日志到队列
void Log::writeLogToQue(std::string data, const char* file, int line, Log_Level level){
    switch(level){
        case DEBUG:
            writeLogToQue_V(data,file, line, "DEBUG");
            break;
        case INFO:
            writeLogToQue_V(data,file, line, "INFO");
            break;
        case WARN:
            writeLogToQue_V(data,file, line, "WARN");
            break;
        case FATAL:
            writeLogToQue_V(data,file, line, "FATAL");
            break;
         default:
            break;
    }
}

//检测文件大小，是否需要进行替换
void Log::replaceLogFile(){
     struct stat buf;
     stat(filename_.c_str(), &buf);
     if(buf.st_size<fileMaxSize_*1024.0*1024)
         return;

     //需要新创建文件
     char newFileName[512] = {0};
     sprintf(newFileName,"%s%s_%s_.log",dirName_.c_str() , filename_.c_str() , MyTime::getTimeStampString_sec().c_str());
        //关闭原有的文件描述符
    close(fd_);
     //打开文件获取文件描述符
    fd_ = open(newFileName, O_RDWR|O_APPEND|O_CREAT, 0666);
    assert(fd_!=-1);

}


//线程函数 ==>将log刷新到磁盘文件
// 队列数量达到maxLogs_时 或 每三秒刷新
void Log::flushLogs(){
    while(1){
        //加锁
        int ret = 0; //是否超时返回
        MutexLockGuard loc(mutexlock_);
        while(que_.size()<maxLogs_ && ret!=ETIMEDOUT){
            if(isExit_){
                cond_notfull.notifyAll();
                return;
            }
            ret = cond_full.timeWait(mutexlock_.getMutex(), 3000);
        }
        if(ret == ETIMEDOUT){
            // std::cout<<"TIMEOUT\n";
        } 
        //检测是否需要使用新log文件
        replaceLogFile();

        while(!que_.empty()){
            std::string str = que_.front();
            write(fd_, str.c_str(), str.size());
            que_.pop();
        }
        if(isExit_){
            cond_notfull.notifyAll();
            return;
        }

        cond_notfull.notifyAll();
    }
}

//将日志写入队列
void Log::writeLogToQue_V(std::string &data, const char* file, int line,const char * levelStr){
    MutexLockGuard loc(mutexlock_);
    //mutexlock_.lock();
    while(que_.size()==maxLogs_){
        cond_notfull.wait(mutexlock_.getMutex());
    }
    //获取当前时间
    char buf[1024] = {0};
    std::string ttimeStr = MyTime::getTimeString();
    sprintf(buf, "[%s] [%s] [filename=%s, line=%d]: %s \n",
            levelStr,
            ttimeStr.c_str(),
            file,
            line,
            data.c_str());
    //std::cout<<"queue: "<<buf<<std::endl;
    que_.push(buf);

    if(que_.size()==maxLogs_){
        cond_full.notifyAll();
    }else{
    
        cond_notfull.notifyAll();
    }

}
