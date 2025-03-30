#include"rpcchannel.h"
#include"google/protobuf/descriptor.h"
#include<string>
#include"header.pb.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include"mprpcapplication.h"
#include"arpa/inet.h"
#include"errno.h"
#include"unistd.h"

/*
从调用端获取调用信息发送给callee
双方约定数据格式
header_size(4) + header_str + arg_str
*/
void mRpcChannel::CallMethod(const google::protobuf:: MethodDescriptor* method,
                         google::protobuf:: RpcController* controller, const google::protobuf::Message* request,
                         google::protobuf:: Message* response,google::protobuf::Closure* done)
{
   const  google::protobuf::ServiceDescriptor *sd = method->service();
   std::string service_name = sd->name();
   std::string service_method = method->name();
   uint32_t arg_size = 0;
   std::string arg_str;
   if(request->SerializeToString(&arg_str)){
      arg_size = arg_str.size();
   }
   else{
     controller->SetFailed("serialize request error!");
    return;
   }
   mprpc::RpcHeader rpcheader;
   rpcheader.set_service_name(service_name);
   rpcheader.set_service_method(service_method);
   rpcheader.set_arg_size(arg_size);

   uint32_t header_size = 0;
   std::string header_str;
   if(rpcheader.SerializeToString(&header_str)){
     header_size = header_str.size();
   }
   else{
    controller->SetFailed("serialize rpc header error!");
    return;
   }
  
   std::string send_str;
   send_str.insert(0,std::string((char*)&header_size,4));
   send_str += header_str;
   send_str += arg_str;

   //打印调试信息
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"header_str: "<<header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"service_method: "<<service_method<<std::endl;
    std::cout<<"arg_str: "<<arg_str<<std::endl;

    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(clientfd == -1){
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        exit(EXIT_FAILURE);
    }
    //std::string ip = MprpcApplication::getinstance().getConfig().load("rpcserverip");
    //uint32_t port = atoi(MprpcApplication::getinstance().getConfig().load("rpcserverport").c_str());

    //从zookeeper上查询该方法的所在服务器信息
   ZookeerperUtil us;
   us.Start();
   std::string method_path = "/" + service_name + "/" + service_method;
   std::string host_data = us.GetData(method_path.c_str());
   if(host_data == ""){
    controller->SetFailed(method_path + " is not exist!");
    return;
   }
   //127.0.0.1:8000
   int idx = host_data.find(":");
   if(idx == -1){
    controller->SetFailed(method_path + " address is invalid!");
    return;
   }
   std::string ip  = host_data.substr(0,idx);
   uint16_t port =atoi(host_data.substr(idx +1 ,host_data.size() - idx).c_str());


    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(-1 == connect(clientfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))){
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if(-1 == send(clientfd,send_str.c_str(),send_str.size(),0)){
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    int  recv_size = 0;
    char recv_buf[1024] = {0};
    if(-1==(recv_size=recv(clientfd,recv_buf,1024,0))){
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

  // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        close(clientfd);
        char errtxt[2048] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);
}