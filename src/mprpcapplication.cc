#include"mprpcapplication.h"
#include<iostream>
#include<unistd.h>
using namespace std;

MprpcConfig MprpcApplication::mpconfig; // 定义 mpconfig
void ShowArgHelp(){
    cout<<"format: command -i<configfile"<<endl;
}

void MprpcApplication::init(int argc,char*argv[]){
   if(argc < 2){
     ShowArgHelp();
     exit(EXIT_FAILURE);
   }
   int c = 0;
   string config_file;
   while((c = getopt(argc,argv,"i:")) != -1){
     switch (c)
     {
     case 'i':
        config_file = optarg;
        break;
     case '?':
        ShowArgHelp();
        exit(EXIT_FAILURE);
    case ':':
        ShowArgHelp();
        exit(EXIT_FAILURE);
     default:
        break;
     }
   }

   //开始读取配置文件,ip,port,zookeeper ip,port
   mpconfig.LoadConfigFile(config_file.c_str());

   // cout<<"rpcserverip:"<<mpconfig.load("rpcserverip")<<endl;
   // cout<<"rpcserverport:"<<mpconfig.load("rpcserverport")<<endl;
   // cout<<"zookeeperip:"<<mpconfig.load("zookeeperip")<<endl;
   // cout<<"zookeeperport:"<<mpconfig.load("zookeeperport")<<endl;

}

 MprpcApplication& MprpcApplication::getinstance(){
     static MprpcApplication app;
     return app;
}

 MprpcConfig& MprpcApplication::getConfig(){
    return mpconfig;
 }