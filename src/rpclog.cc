#include"rpclog.h"
#include<time.h>
#include<iostream>


 Logger& Logger::getinstance(){
   static Logger lp;
   return lp;
 }
 //设置日志等级
 void Logger::setLogLevel(loglevel level){
     m_level = level;
}
    //写日志
 void Logger:: log(std::string msg){
     m_logque.push(msg);
}


Logger:: Logger (){
    //启动专门写日志线程
    std::thread writeLogTask([&](){
         for(;;){
         time_t now = time(nullptr);
         tm*nowtime = localtime(&now);

        char file_name[128];
        sprintf(file_name,"%d_%d_%d_log.txt",nowtime->tm_year +1,nowtime->tm_mon + 1,nowtime->tm_mday);
        FILE *fl = fopen(file_name,"a+");
        if(fl == nullptr){
            std::cout << "logger file : " << file_name << " open error!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string msg = m_logque.pop();

         char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                    nowtime->tm_hour, 
                    nowtime->tm_min, 
                    nowtime->tm_sec,
                    (m_level == INFO ? "info" : "error"));
         msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), fl);
            fclose(fl);

    }
    });
    //分离线程，异步执行
    writeLogTask.detach();
}