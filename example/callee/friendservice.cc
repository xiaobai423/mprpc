#include<iostream>
#include<string>
#include"friend.pb.h"
#include"mprpcapplication.h"
#include"provide.h"
#include<vector>
using namespace std;

using namespace fixbug;

class FriendService:public FiendServiceRpc{
    public:
    std::vector<string>GetFriendList(int userid){
         vector<string>friendlist;
         friendlist.push_back("苏嘉航");
         friendlist.push_back("闵哲");
         friendlist.push_back("康展翼");
         friendlist.push_back("李昱辉");
         friendlist.push_back("王震");
         return friendlist;
    }

void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
{
   int userid = request->userid();

   vector<string>vec = GetFriendList(userid);

    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
   for(auto it:vec){
    response->add_friends(it);
   }

   done->Run();
}
};

int main(int argc,char *argv[]){
   MprpcApplication::init(argc,argv);

   RpcProvider rpc;
   rpc.NotifyService(new FriendService());
   rpc.run();
   return 0;
}