#include<iostream>
#include<string>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"provide.h"
using namespace std;

using namespace fixbug;
/*业务代码，得到对端需要调用的函数*/
class UserService:public UserServiceRpc{
    public:
    //示例1
    bool Log(string name,string pwd){
       cout<<"doing login service"<<endl;
       cout<<"name: "<<name<<"password: "<<pwd<<endl;
       return true;
    }
    void login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::loginResponse* response,
                       ::google::protobuf::Closure* done)
        {
            //通过解析request得到请求参数
           string name  = request->name();
           string pwd = request->pwd();

           //执行需要调用的函数
           bool res = Log(name,pwd);

           //返回需要响应的信息
           response->mutable_result()->set_errcode(0);
           response->mutable_result()->set_errmsg("");
           response->set_success(res);

           //执行回调操作，序列化响应数据和发送(由框架完成)
           done->Run();
        }
    //示例2
    uint32_t Regist(string name,string pwd){
        std::cout<<"Username: "<<name<<"Userpassword: "<<pwd<<std::endl;
        return 1;
    }
void reg(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegRequest* request,
                       ::fixbug::RegResponse* response,
                       ::google::protobuf::Closure* done)
{
   std::string name = request->name();
   std::string pwd = request->pwd();

   uint32_t id = Regist(name,pwd);
   
   response->set_id(id);
   response->mutable_result()->set_errcode(0);
   response->mutable_result()->set_errmsg("");

   done->Run();
                               
}
};

int main(int argc,char *argv[]){
    //初始化框架
    MprpcApplication::init(argc,argv);
 
    RpcProvider rp;
    //把UserService发布到rpc节点上
    rp.NotifyService(new UserService());

    //启动rpc发布节点，run以后进入阻塞，等待远程rpc调用请求
    rp.run();
    return 0;
}