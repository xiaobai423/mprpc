#pragma once
#include"google/protobuf/service.h"
#include"muduo/net/TcpServer.h"
#include"muduo/net/InetAddress.h"
#include"muduo/net/EventLoop.h"
#include"muduo/net/TcpConnection.h"
#include<google/protobuf/descriptor.h>
#include<unordered_map>
#include<functional>
class RpcProvider{
    public:
    //框架提供给外部使用者，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    //启动rpc远程网络服务
    void run();

    private:
    muduo::net::EventLoop m_eventloop;

    //保存服务对象信息
    struct ServiceInfo{
        google::protobuf::Service *m_service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*>m_methodMap;
    };
    //存储注册成功服务对象的所有信息
    std::unordered_map<std::string,ServiceInfo>m_serviceMap;
   
   void OnConnect(const muduo::net:: TcpConnectionPtr& conn);


   void OnMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp receiveTime);

   void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message*);

};