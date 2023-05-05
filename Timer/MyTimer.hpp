#ifndef MYTIMER_H
#define MYTIMER_H

#include<map>
#include<sys/select.h>
#include <pthread.h>
#include<string>
#include<memory>
#include<iostream>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include"../lock_cond/lock_cond.hpp"

//任务map类型
using taskMapType = std::map<pthread_t,struct task_msg> ; 
//任务函数类型
using taskCbType = void (*)(void*);
    
struct task_msg{
    taskCbType func_; //任务执行函数
    std::string desc_; //任务的描述
    int nums_; //任务执行的次数, -1表示一直执行
    long times_; //任务间隔时间（ms）
    std::shared_ptr<void> arg; //为了动态管理用户传来arg的内存
};
     
struct this_arg{
    void* ptr_this;
    void* arg;
};

//定时器类==>单例
class MyTimer{
public:
   
    //获取唯一实例
    static MyTimer* getInstance(){
        return instance;
    }
    //添加任务 times:时间间隔 taskCb:任务函数 arg:任务参数  nums:任务执行次数  desc:任务描述
    bool addTask(long times, taskCbType taskCb, std::shared_ptr<void> arg,int nums = -1, std::string desc = "");

    //删除任务
    void delTask(pthread_t taskId);

    //获取所有任务Map
    const taskMapType*  getTaskMap();
    
private:
    MyTimer()
    {
            
    }
    ~MyTimer(){
    
    }

    //回调函数
    void timer_cb(void* arg);

    //定时函数 (ms)
    void selectSleep(long ms);

    static void* cb_func(void* args);
   
    //存储所有定时任务
    taskMapType task_Map_; 
    //单例
    static MyTimer* instance; //俄汉式
    //保证task_Map的线程安全
    MutexLock mutex_;

};


#endif
