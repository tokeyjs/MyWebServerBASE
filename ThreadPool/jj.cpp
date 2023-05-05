#include"threadpool.hpp"
#include<unistd.h>

class Task__:public MyTask {
public:
    Any run(){
        usleep(500000);
        std::cout<<"ppp"<<std::endl;

        Any a = "451";
        return a;
    }

};


int main(){
    ThreadPool pool;
    pool.setModel(CACHE);
    pool.setMaxTaskNums(3);

    for(int i=0;i<40;i++){
        pool.commitTask(make_shared<Task__>());
    }

    


    sleep(15);
    return 0;

}
