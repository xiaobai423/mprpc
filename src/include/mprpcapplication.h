#pragma once

#include"mprpcconfig.h"
#include"rpcchannel.h"
#include"controller.h"
#include"rpclog.h"
#include"zookeeperutil.h"
//用于初始化rpc框架
class MprpcApplication{
   public:
   static void init(int argc,char*argv[]);

   static MprpcApplication&getinstance();

   //获取配置文件
   static MprpcConfig&getConfig();
   private:
   MprpcApplication(){}
   MprpcApplication(const MprpcApplication&) = delete;
   MprpcApplication(MprpcApplication&&) = delete;

   static MprpcConfig mpconfig;
};