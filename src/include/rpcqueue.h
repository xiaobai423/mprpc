#pragma once
#include<queue>
#include<mutex>
#include<condition_variable>
#include<thread>
using namespace std;

template<class T>
class LogQueue{
    public:
    void push(const T&data){
        lock_guard<mutex>lock(m_mutex);
        m_que.push(data);
        m_condition.notify_one();
    }
    T pop(){
        std::unique_lock<std::mutex>lock(m_mutex);
        while(m_que.empty()){
           m_condition.wait(lock);
        }
        T data =  m_que.front();
        m_que.pop();
        return data;
    }
    private:
   std::queue<T>m_que;
   std::mutex m_mutex;
   std::condition_variable m_condition;
};