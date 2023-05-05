#ifndef MYANY_H
#define MYANY_H

#include<memory>
#include<iostream>
using namespace std;

//Any 类

class Any{
public:
    template<typename T>
    Any(T val)
        :val_(make_unique<Value<T>>(val))
    {}

    Any() = default;
    ~Any() = default;
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;

    Any(Any&&);
    Any& operator=(Any&&) = default;

    template<typename T>
    T& get(){
        Value<T> *p = dynamic_cast<Value<T>*>(val_.get());
        if(p == nullptr)
            throw "type error!";
        return *(p->data_);
    }
private:
    class BaseValue{
    public:
        virtual ~BaseValue(){}
    };

    template<typename T>
    class Value: public BaseValue{
    public:
        Value(T data)
            :data_(data)
        {}

        T data_;
    };

    unique_ptr<BaseValue> val_;
};


#endif
