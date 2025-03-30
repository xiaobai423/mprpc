#pragma once

#include"zookeeper/zookeeper.h"
#include"mprpcapplication.h"
#include<semaphore.h>
#include<iostream>

class ZookeerperUtil{
    public:
    ZookeerperUtil();
    ~ZookeerperUtil();

    //启动zookeeper客户端，与server连接
    void Start();
    
    //创建节点
    void Create(const char *path,const char *data,int datlen,int state = 0);

    //根据路径查询节点值
    std::string GetData(const char *path);


    private:
    zhandle_t *m_handle;
};
