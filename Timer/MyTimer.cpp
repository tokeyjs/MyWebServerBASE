#include"MyTimer.hpp"

MyTimer* MyTimer::instance = new MyTimer();



//定时函数 (ms)
void MyTimer::selectSleep(long ms){
    struct timeval val;
    val.tv_sec = ms/1000;
    val.tv_usec = (ms%1000)*1000;

    select(0, NULL, NULL, NULL, &val);
} 

//回调函数
void MyTimer::timer_cb(void *arg){
    int num = 1;
    pthread_t tid = pthread_self();
    taskCbType func;
    long times_;
    struct task_msg task;
    {
        MutexLockGuard loc(mutex_);
        task = task_Map_.find(tid)->second;
    }
    num = task.nums_;
    func = task.func_;
    times_ = task.times_;

    while(num<0||num>0){
        //随眠
        selectSleep(times_);
        //执行任务
        func(arg);

        num--;
    }
}

void* MyTimer::cb_func(void * args){
    struct this_arg* p = (struct this_arg*)args;
    MyTimer *pthis = (MyTimer*)p->ptr_this;
    void* arg = p->arg;
    delete p;                   
    pthis->timer_cb(arg);
    return NULL;
}


//添加任务 times:时间间隔(ms) taskCb:任务函数 arg:任务参数(请使用make_shared<T>())  nums:任务执行次数  desc:任务描述
bool MyTimer::addTask(long times,taskCbType taskCb, std::shared_ptr<void> arg,int nums, std::string desc){
    MutexLockGuard loc(mutex_);
    struct task_msg task;
    task.desc_ = desc;
    task.func_ = taskCb;
    task.nums_ = nums;
    task.times_ = times;
    task.arg = arg;  //饮用数加一，保证在任务推出前arg都能有用
    
    auto ptr = new struct this_arg;
    ptr->arg = arg.get();
    ptr->ptr_this = (void *)this;
    //创建线程
    pthread_t p;
    pthread_create(&p, NULL, cb_func, (void *)ptr);
    pthread_detach(p);

    //加入Map
    task_Map_[p] = task;

    return true;
}

//删除任务
void MyTimer::delTask(pthread_t taskId){
    MutexLockGuard loc(mutex_);
    auto it =  task_Map_.find(taskId);
    if(it!=task_Map_.end()){
        task_Map_.erase(it);
        pthread_cancel(taskId);
    }
}

//获取所有任务Map
const taskMapType*  MyTimer::getTaskMap(){
    return &task_Map_;
}

