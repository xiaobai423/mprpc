#pragma once

#include<string>
#include<unordered_map>
using namespace std;
class MprpcConfig{
   public:
   //负责解析加载配置文件
   void LoadConfigFile(const char *config_file);

   //查询配置信息
   string load(const string& key);

   void Trim(string & bufs);
   private:
    unordered_map<string,string>m_configMap;
};