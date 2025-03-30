#pragma once
#include <google/protobuf/service.h>
#include <string>

class Controller:public google::protobuf:: RpcController {
 public:
   Controller();
   ~Controller();

 void Reset();


  bool Failed()const;

 std::string ErrorText()const;


 void StartCancel();

 
void  SetFailed(const std::string& reason);

 bool IsCanceled() const;


 void NotifyOnCancel(::google::protobuf::Closure* callback);

 private:
   bool failed = false;
   std::string errtxt;
};