#include"zookeeperutil.h"

void global_watcher(zhandle_t *zh, int type, 
    int state, const char *path,void *watcherCtx){
    if(type == ZOO_SESSION_EVENT){//返回消息类型数据
        if(state == ZOO_CONNECTED_STATE){//client和server连接成功
           sem_t *sem = (sem_t*)zoo_get_context(zh);

           sem_post(sem);
        }
}
}

ZookeerperUtil::ZookeerperUtil():m_handle(nullptr){

}
ZookeerperUtil::~ ZookeerperUtil(){
  if(m_handle != nullptr){
    zookeeper_close(m_handle);
  }
}

void ZookeerperUtil::Start(){
    std::string zookeeperip = MprpcApplication::getinstance().getConfig().load("zookeeperip");
    std::string zookeerperport = MprpcApplication::getinstance().getConfig().load("zookeeperport");
    std::string constr = zookeeperip + ":" + zookeerperport;
    //客户端创建三个线程
    //api调用线程
    //网络io线程
    //回调线程golab_watch
    //m_handle创建成功并不代表连接成功，golab_watch回调成功才是连接成功
    m_handle = zookeeper_init(constr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_handle == nullptr){
        std::cout<<"handle init error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_handle,&sem);

    sem_wait(&sem);

    std::cout<<"zookeeper init success"<<std::endl;
}
void ZookeerperUtil::Create(const char *path,const char *data,int datlen,int state){
    char path_buffer[128];
    int buf_size = sizeof(path_buffer);
    int flag;
    //检查是否该路径已创建过节点
    flag = zoo_exists(m_handle,path,0,nullptr);
    if(ZNONODE == flag){//节点不存在
        flag = zoo_create(m_handle,path,data,datlen,&ZOO_OPEN_ACL_UNSAFE,state,path_buffer,buf_size);
        if(flag == ZOK){
           std::cout<<"node create success.....path:"<<path<<std::endl;
        }
        else{
            std::cout<<"flag :"<<flag<<endl;
            std::cout<<"node create error path:"<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }


}
std::string ZookeerperUtil::GetData(const char *path){
    char buffer[64];
    int bufflen = sizeof(buffer);
    int flag = zoo_get(m_handle,path,0,buffer,&bufflen,nullptr);
    if(flag != ZOK){
        std::cout<<"get znode error :"<<path<<std::endl;
        return "";
    }
    else{
        return buffer;
    }
}

