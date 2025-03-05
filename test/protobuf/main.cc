#include"test.pb.h"
#include<iostream>
using namespace fixbug;
using namespace std;



int main(){
    //类中套类
    // Response rq;
    // Result *rs = rq.mutable_errno();
    // rs->set_errno(0);
    // rs->set_errmsg("正确");
    // rq.set_result(1);
    
    getfriendlist fd;
    Result *rd = fd.mutable_errno_code1();
    rd->set_errmsg("hai");
    rd->set_errno_ack(1);

    User *us;
    us = fd.add_users();
    us->set_age(10);
    us->set_name("lisi");

    User *us1;
    us1 = fd.add_users();
    us1->set_age(11);
    us1->set_name("wangwu");

    cout<<fd.users_size()<<endl;

    return 0;
}

int main1(){
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    //数据序列化
    std::string send;
    if(req.SerializePartialToString(&send)){
        std::cout<<send<<endl;
    }

    //数据反序列化
    LoginRequest req1;
    if(req1.ParseFromString(send)){
       cout<<req1.name()<<endl;
       cout<<req1.pwd()<<endl;
    }


    return 0;
}
