#include"mprpcconfig.h"

#include<iostream>
using namespace std;
 //负责解析加载配置文件
void MprpcConfig:: LoadConfigFile(const char *config_file){
         FILE * fl = fopen(config_file,"r");
         if(fl == nullptr){
            cout<<config_file<<"is not exist"<<endl;
            exit(EXIT_FAILURE);
         }
         while(!feof(fl)){
         char buf[512] = {0};
         fgets(buf,512,fl);

         string rc_buf(buf);

         Trim(rc_buf);
           //跳过注释行或空行
        if(rc_buf[0] =='#' || rc_buf.empty()){
            continue;
         }
        int idx = rc_buf.find('=');
        if(idx == -1){
            continue;
        }

        string value;
        string key;
        key = rc_buf.substr(0,idx);
        Trim(key);
        int endix = rc_buf.find('\n',idx);
        value = rc_buf.substr(idx + 1,endix - idx -1);
        Trim(value);
        m_configMap.insert({key,value});
    }
    //fclose(fl);
}

   //查询配置信息
string MprpcConfig::load(const string& key){
      auto it = m_configMap.find(key);
      if(it == m_configMap.end()){
        return "";
      }
      return it->second;
   }

void MprpcConfig::Trim(string & rc_buf){
    //去掉前面多余的空格
    int idx = rc_buf.find_first_not_of(' ');
         if(idx != -1){
             rc_buf = rc_buf.substr(idx,rc_buf.size() - idx);
         }

         //去掉后面的空格
        idx = rc_buf.find_last_not_of(' ');
         if(idx != -1){
            rc_buf =  rc_buf.substr(0,idx + 1);
         }
}