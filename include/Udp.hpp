#ifndef SRC_UDP_H
#define SRC_UDP_H

#pragma once

//暂时以int表示ip, 有需要则封装成类
using IP = int;

namespace port
{
  const constexpr int kCLI_LOGIN_  = 20800;
  const constexpr int kCLI_BEAT_   = 20801;
  const constexpr int kMIR_BEAT_   = 20802;
  const constexpr int kINS_MSG_    = 20803; 
}

namespace udp
{
  void loop();

  void recv();
  void send();
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
