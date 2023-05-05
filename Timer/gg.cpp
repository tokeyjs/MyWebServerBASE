#include<iostream>
#include"../Time/mytime.hpp"
#include"MyTimer.hpp"
#include<memory>

void fun(void* arg){
    std::cout<<*((int*)arg)<<"  "<<MyTime::getTimeStampString_sec()<<std::endl;
}


int main(){
    


    MyTimer::getInstance()->addTask(3000, fun, std::make_shared<int>(50), -1);

    while(1){
    
    }

    return 0;
}
