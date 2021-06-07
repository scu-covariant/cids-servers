#ifndef SRC_UDP_H
#define SRC_UDP_H

#pragma once

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>

//暂时以int表示ip, 有需要则封装成类
using IP = int;

namespace port
{
    const constexpr size_t kCLI_LOGIN_  = 20800;
    const constexpr size_t kCLI_BEAT_   = 20801;
    const constexpr size_t kMIR_BEAT_   = 20802;
    const constexpr size_t kINS_MSG_    = 20803;
}

namespace protocal
{
    //以字节计算的每个包的大小和协议中每一部分的大小, 二级缩进的和应该等于第一级缩进
    const constexpr size_t kCLI_LOGIN_PACSIZE_      = 8;
    const constexpr size_t kCLI_LOGIN_UID_    = 7;
    const constexpr size_t kCLI_LOGIN_STATE_  = 1;

    const constexpr size_t kCENT_RESPONSE_PACSIZE_  = 4;

    const constexpr size_t kMIR_BEAT_PACSIZE_       = 8;
    const constexpr size_t kMIR_BEAT_IP_      = 4;
    const constexpr size_t kMIR_BEAT_LOAD_    = 4;
}

#endif //SRC_UDP_H

/*
class IP
{
private:
    union //IP地址共占4个字节
    {
        struct //这是一个由4个字节构成的匿名结构体
        {
            unsigned char seg0;
            unsigned char seg1;
            unsigned char seg2;
            unsigned char seg3;
        };  //4字节的IP地址可以看作4部分，每部分1字节
        unsigned int address; //4字节的IP地址可以看成一个4字节的整体
    };
public:
    IP(int=0,int=0,int=0,int=0);  //构造函数
    void showIP();  //用四段法显示IP地址
    bool sameSubnet(const IP &ip, const IP &mark);  //判断是否处于同一子网
    char whatKind();  //返回属于哪一类网络
};

原文链接：https://blog.csdn.net/chongshangyunxiao321/article/details/51055658
*/
