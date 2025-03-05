#pragma once

#include"user.pb.h"
#include"google/protobuf/descriptor.h"

class mRpcChannel:public google::protobuf::RpcChannel
{
    public:
    //重写需要调用的callmethod方法
    void CallMethod(const google::protobuf:: MethodDescriptor* method,
                         google::protobuf:: RpcController* controller, const google::protobuf::Message* request,
                         google::protobuf:: Message* response,google::protobuf::Closure* done);
};