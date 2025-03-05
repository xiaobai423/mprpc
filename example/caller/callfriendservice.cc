#include<iostream>
#include"friend.pb.h"
#include"mprpcapplication.h"

int main(int argc,char *argv[]){
     MprpcApplication::init(argc,argv);
     
     fixbug::FiendServiceRpc_Stub stub(new mRpcChannel());
     fixbug::GetFriendsListRequest rq;
     rq.set_userid(501);

     fixbug::GetFriendsListResponse rs;
    Controller controller;
     
     stub.GetFriendsList(&controller,&rq,&rs,nullptr);
     if(controller.Failed()){
        std::cout<<"rpc error :"<<controller.ErrorText()<<std::endl;
     }
     else{
    if(0 == rs.result().errcode()){
         //调用成功
         int size = rs.friends_size();
        for(int i = 0;i<size;i++){
            std::cout<<"编号: "<<i+1<<" 姓名: "<<rs.friends(i)<<std::endl;
        }
    }
    else{
        std::cout<<"rpc getfriend failed: "<<rs.result().errmsg()<<std::endl;
    }
     }

}