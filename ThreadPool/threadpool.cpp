#include"threadpool.hpp"

ThreadPool::ThreadPool(int max_task_nums,PoolModel model , int init_threads_nums , int max_threads_nums )
    :isExit_(false)
    ,task_nums_(0)
     ,max_task_nums_(max_task_nums)
     ,model_(model)
     ,isStart(false)
     ,init_threads_nums_(init_threads_nums)
     ,threads_nums_(0)
     ,max_threads_nums_(max_threads_nums)
     ,idle_thread_num_(0)
{
    //初始化线程
    initThreads(init_threads_nums_);

}

ThreadPool::~ThreadPool(){

    MutexLockGuard loc(mutex_);

    isExit_ = true;

    cond_empty_.notifyAll();
    while(threads_nums_>0){
        cond_empty_.wait(mutex_.getMutex());
    }

    //cout<<"threads = "<<threads_nums_<<endl;
}


//设置线程池运行模式
bool ThreadPool::setModel(PoolModel mod){
    if(isStart)
        return false;
    model_ = mod;
    return true;
}

//设置最大线程数量
bool ThreadPool::setMaxThreadsNums(int num){
     if(isStart||num<=0)
         return false;
     max_threads_nums_ = num;
     return true;
}

//设置任务队列容量
bool ThreadPool::setMaxTaskNums(int num){
    if(isStart||num<=0)
        return false;
    max_task_nums_ = num;
    return true;
}



void* ThreadPool::threadCb(void *arg){
    ThreadPool* tp = (ThreadPool*)(arg);
    tp->func();
    return NULL;
}

//线程函数==>不断从任务队列中获取任务
void ThreadPool::func(){
    while(1){
        time_t startTime = MyTime::getTimeStamp_sec();
        time_t endTime;
        shared_ptr<MyTask> task;

        {
            MutexLockGuard loc(mutex_);
            idle_thread_num_++;
            while(task_nums_<=0){
                cond_empty_.wait(mutex_.getMutex());
                if(isExit_){
                    threadsSet_.erase(pthread_self());
                    threads_nums_--;
                    cond_empty_.notifyAll();
                    return;
                }
            }

            //获取任务
            task = taskQue_.front();
            taskQue_.pop();
            task_nums_--;
            idle_thread_num_--;

            cond_full_.notifyAll();

            endTime = MyTime::getTimeStamp_sec();
        }
        //开始执行任务
        task->exec();

        if(isExit_){
                threadsSet_.erase(pthread_self());
                threads_nums_--;
                cond_empty_.notifyAll();
                return;
        } 

        //查看是否退出空闲线程
        if(model_==CACHE&&(endTime-startTime)>MAX_IDLE_TIME&&threads_nums_>init_threads_nums_){
            threadsSet_.erase(pthread_self());
            threads_nums_--;
            return; 
        }
    }


}

//提交任务
bool ThreadPool::commitTask(shared_ptr<MyTask> task){
    MutexLockGuard loc(mutex_);
    isStart = true;
    int retcode = 0;
    while(task_nums_>=max_task_nums_&&retcode!=ETIMEDOUT){
        retcode = cond_full_.timeWait(mutex_.getMutex(), 600);
        
        if(isExit_)
            return false;
    }

    //超时
    if(retcode==ETIMEDOUT){
        //提交任务失败
            //查看是否需要增加线程数量
        if(model_==CACHE&&threads_nums_<max_threads_nums_&&idle_thread_num_<=1){
            //增加线程
            pthread_t p;
            pthread_create(&p, NULL, ThreadPool::threadCb, (void*)this);
            pthread_detach(p);
            //添加到set中
            threadsSet_.insert(p);
            threads_nums_ ++;
        }

        return false;
    }else{
        //预提交任务成功
        taskQue_.push(task);
        task_nums_++;

        //查看是否需要增加线程数量
        if(model_==CACHE&&threads_nums_<max_threads_nums_&&idle_thread_num_<=1){
            //增加线程
            pthread_t p;
            pthread_create(&p, NULL, ThreadPool::threadCb, (void*)this);
            pthread_detach(p);
            //添加到set中
            threadsSet_.insert(p);
            threads_nums_ ++;
        }


        cond_empty_.notifyAll();

        return true;
    }

}


//初始化nums个线程
void ThreadPool::initThreads(int nums){
    MutexLockGuard loc(mutex_);
    while(nums--){
        pthread_t p;
        pthread_create(&p, NULL, ThreadPool::threadCb, (void*)this);
        pthread_detach(p);

        //添加到队列中
        threadsSet_.insert(p);
        threads_nums_ ++; 
    }
   
}
