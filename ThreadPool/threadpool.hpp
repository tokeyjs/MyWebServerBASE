#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<string>
#include<memory>
#include"../lock_cond/lock_cond.hpp"
#include<queue>
#include<unordered_set>
#include"mytask.hpp"
#include"../Time/mytime.hpp"
using namespace std;

//初始化线程池线程数量
const int INIT_THREADS_NUMS = 2;
//最大线程数量
const int MAX_THREADS_NUMS = 6;
//任务队列最大数量
const int MAX_TASK_NUMS = 256;
//最大空闲时长(s)(CACHE模式下一个线程空闲时间超过此值就有可能会被退出)
const int MAX_IDLE_TIME = 2; 

enum PoolModel{
  FIXED,//固定线程数量
  CACHE //动态调控线程数量
};

//线程池
class ThreadPool{
public:
    ThreadPool(int max_task_nums=MAX_TASK_NUMS,PoolModel model = FIXED, int init_threads_nums = INIT_THREADS_NUMS, int max_threads_nums = MAX_THREADS_NUMS);
    ~ThreadPool();

    static void* threadCb(void* arg);

    //提交任务
    bool commitTask(shared_ptr<MyTask> task);


    //设置线程池运行模式
    bool setModel(PoolModel mod);

    //设置最大线程数量
    bool setMaxThreadsNums(int num);

    //设置任务队列容量
    bool setMaxTaskNums(int num);


private:
    //初始化各个线程
    void initThreads(int nums);

    //线程执行函数回调函数
    void func();

    //线程池状态
    bool isExit_; 

    //任务队列
    queue<shared_ptr<MyTask>> taskQue_;
    int task_nums_; //当前任务队列任务数量
    int max_task_nums_; //任务队列容量

    //线程池模式
    PoolModel model_;
    //线程池是否开始（是否已经提交任务）
    //如果开始了就不能进行参数配置
    bool isStart; 

    //线程队列
    // queue<pthread_t> threadsQue_;
    unordered_set<pthread_t> threadsSet_;

    int init_threads_nums_; //初始线程数量
    int threads_nums_; //当前线程数量
    int max_threads_nums_; //最大线程数量
    int idle_thread_num_; //空闲线程数量


    MutexLock mutex_; //保证任务队列的线程安全
    Cond cond_empty_; //保证线程安全，并进行线程间通信    
    Cond cond_full_; //保证线程安全，并进行线程间通信    
};


#endif


