#ifndef MYTASK_H
#define MYTASK_H

#include"myany.hpp"
#include"../lock_cond/lock_cond.hpp"

//线程任务基类（用户编写其子类）
class MyTask{
public:
    MyTask(){}
    virtual ~MyTask(){}

    virtual Any run(){
        //线程执行函数
        return Any();
    }

    void exec(){
        //执行
        result_ =  run();
        //保存返回值
        sem_.post();
    }

    Any& getResult(){
        //获取任务返回值
        sem_.wait();
        return result_;
    }

private:
    Sem sem_;
    Any result_; //任务返回值
};


#endif


