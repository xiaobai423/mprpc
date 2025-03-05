#include<iostream>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"rpcchannel.h"

int main(int argc,char *argv[]){
    //caller调用端启动rpc框架
    MprpcApplication::init(argc,argv);

    //远程调用rpc方法
    fixbug::UserServiceRpc_Stub stub(new mRpcChannel());

    fixbug::LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    fixbug::loginResponse res;
    stub.login(nullptr,&req,&res,nullptr);//最终调用rpcchannel->callmethod
    
 
    if(0 == res.result().errcode()){
         //调用成功
        std::cout<<"rpc login success: "<<res.success()<<std::endl;

    }
    else{
        std::cout<<"rpc login failed: "<<res.result().errmsg()<<std::endl;
    }
//示例2
   fixbug::RegRequest rq;
   rq.set_name("lisi");
   rq.set_pwd("666666");

   fixbug::RegResponse rs;
   stub.reg(nullptr,&rq,&rs,nullptr);

   if(0 == rs.result().errcode()){
         //调用成功
        std::cout<<"rpc reg id: "<<rs.id()<<std::endl;

    }
    else{
        std::cout<<"rpc reg failed: "<<rs.result().errmsg()<<std::endl;
    }

   
    return 0;
}