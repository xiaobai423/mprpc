#include"provide.h"
#include<string>
#include<mprpcapplication.h>
#include"header.pb.h"

//框架提供给外部使用者，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service){
      //创建存储信息的实例
      ServiceInfo  service_info;
      //获取服务对象的描述信息
      const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();

      //获取服务对象的名字
      std::string service_name = pserviceDesc->name();

      //获取服务对象的方法数量
      int methodCnt = pserviceDesc->method_count();

      for(int i = 0;i < methodCnt ;i++){
        //获取服务对象指定方法的抽象描述
        const google::protobuf::MethodDescriptor * pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        //存储对象方法
        service_info.m_methodMap.insert({method_name,pmethodDesc});
      }

      //存储对象
      service_info.m_service = service;
      m_serviceMap.insert({service_name,service_info});
}

    //启动rpc远程网络服务
void RpcProvider:: run(){
    std::string ip = MprpcApplication::getinstance().getConfig().load("rpcserverip");
    uint32_t port = atoi(MprpcApplication::getinstance().getConfig().load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    
    //创建server
    muduo::net::TcpServer server(&m_eventloop,address,"RpcProvider");

    //绑定回调
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnect,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    
    server.setThreadNum(4);

     // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZookeerperUtil us;
    us.Start();
    // service_name为永久性节点    method_name为临时性节点
    for(auto & sp:m_serviceMap){
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        us.Create(service_path.c_str(), nullptr, 0);
        for(auto &mp:sp.second.m_methodMap){
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_data[128] = {0};
            sprintf(method_data,"%s:%d",ip.c_str(),port);
            us.Create(method_path.c_str(),method_data,strlen(method_data),ZOO_EPHEMERAL);
        }
    }


    cout<<ip<<"服务已启动 "<<port<<endl;

    server.start();
    m_eventloop.loop();

}
void RpcProvider:: OnConnect(const muduo::net::TcpConnectionPtr& conn){
      if(!conn->connected()){
        //断开连接
        conn->shutdown();
    }
}

/*
收发双方约定好文件格式
header_size(4) + header_str + args
header_str(service_name + method + arg_size)
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp receiveTime)
{
     // 网络上接收的远程rpc调用请求的字符流    Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);
    
    //读取service_name + method + arg_size
    std::string header_str = recv_buf.substr(4,header_size);



    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string service_method;
    uint32_t arg_size;
    if(rpcHeader.ParseFromString(header_str)){
        //数据反序列化
        service_name = rpcHeader.service_name();
        service_method = rpcHeader.service_method();
        arg_size = rpcHeader.arg_size();
    }
    else{
        std::cout<<"rpc_header_str"<<header_str<<"parse error"<<endl;
        return;
    }
    //获取rpc方法参数
    std::string arg_str = recv_buf.substr(4 + header_size,arg_size);

    //打印调试信息
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"header_str: "<<header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"service_method: "<<service_method<<std::endl;
    std::cout<<"arg_str: "<<arg_str<<std::endl;

    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end()){
       std::cout<<service_name<<"is not existed"<<std::endl;
       return;
    }
    auto mit = it->second.m_methodMap.find(service_method);
    if(mit == it->second.m_methodMap.end()){
        std::cout<<service_name<<": "<<service_method<<"is not existed"<<std::endl;
        return;
    }

    //获取对应的对象和方法，返回给protobuf
    google::protobuf::Service *service = it->second.m_service;//Userservice
    
    const google::protobuf::MethodDescriptor *method = mit->second;//Login

    //生成rpc方法调用的request和response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(arg_str)){
        std::cout<<"request parse error :"<<arg_str<<std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //Closue绑定回调
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message *>(this,&RpcProvider:: SendRpcResponse,conn,response);

    //注册回调，向protobuf返回调用信息,callee得到信息处理方法
    service->CallMethod(method,nullptr,request,response,done);
}
//将打包好的数据发送给callee
void RpcProvider:: SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message*response){
    std::string response_str;
    //数据序列化
    if(response->SerializeToString(&response_str)){
       conn->send(response_str);
    }
    else{
        std::cout<<"serize response_str error"<<std::endl;
    }

    conn->shutdown();
}