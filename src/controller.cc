#include"controller.h"

 Controller::Controller(){
    failed = false;
    errtxt = "";
 }
 Controller::~Controller(){}

 void Controller::Reset(){
    failed = false;
    errtxt = "";
 }


  bool Controller::Failed()const{
      return failed;
  }

 std::string Controller::ErrorText()const{
    return errtxt;
 }


 void Controller::StartCancel(){}

 
void Controller:: SetFailed(const std::string& reason){
     failed = true;
     errtxt = reason;
}

 bool Controller:: IsCanceled() const{
    return false;
 }


 void Controller::NotifyOnCancel(google::protobuf::Closure* callback){}