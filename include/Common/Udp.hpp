#ifndef SRC_UDP_H
#define SRC_UDP_H

#pragma once

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> /* struct hostent */
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>


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

struct IP {
    unsigned char seg0;
    unsigned char seg1;
    unsigned char seg2;
    unsigned char seg3;

    IP(int i0 = 0, int i1 = 0, int i2 = 0, int i3 = 0) : seg0(i0), seg1(i1), seg2(i2), seg3(i3) {};

    IP(std::string addr) {
        std::istringstream iss(addr);
        std::string buffer;
        std::vector<unsigned char> res;
        while (std::getline(iss, buffer, '.')) {
            res.push_back((unsigned char) (std::stoi(buffer)));
        }
        if (res.size() != 4) {
            //异常
        }
        this->seg0 = res[0];
        this->seg1 = res[1];
        this->seg2 = res[2];
        this->seg3 = res[3];
    }

    std::string to_string()
    {
        return std::to_string(this->seg0) + '.' + std::to_string(this->seg1) + '.' + std::to_string(this->seg2) + '.' + std::to_string(this->seg3);
    }

    bool operator==(const IP& ip)const{
        return this->seg0 == ip.seg0
        && this->seg1 == ip.seg1
        && this->seg2 == ip.seg2
       && this->seg3 == ip.seg3;
    }

    IP& operator=(const IP& rhs){
        if(this == &rhs)
            return *this;
        this->seg0 = rhs.seg0;
        this->seg1 = rhs.seg1;
        this->seg2 = rhs.seg2;
        this->seg3 = rhs.seg3;
        return *this;
    }
};

namespace std {
    template<>
    struct hash<IP> : public __hash_base<size_t, IP>
    {
        size_t operator()(const IP &ip) const noexcept    //这个const noexpect一定要写上去
        {
            return (((size_t)ip.seg0) << 24)  + (((size_t)ip.seg1) << 16)  + (((size_t)ip.seg2) << 8)  + ((size_t)ip.seg3)  ;
        }
    };
}


static bool GetHostInfo(std::string& hostName, std::string& Ip) {
    char name[256];
    gethostname(name, sizeof(name));
    hostName = name;

    struct hostent* host = gethostbyname(name);
    char ipStr[32];
    const char* ret = inet_ntop(host->h_addrtype, host->h_addr_list[0], ipStr, sizeof(ipStr));
    if (NULL==ret) {
//        std::cout << "hostname transform to ip failed";
        return false;
    }
    Ip = ipStr;
    return true;
}




#endif //SRC_UDP_H


