#ifndef SRC_MIRROR_MIRROR_H
#define SRC_MIRROR_MIRROR_H


#include "muduo/base/ThreadPool.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"

#include "../Telemeter.hpp"
#include "../Type.hpp"  //lock free queue -> lfqueue

#include "ClassInfo.h"
#include "Message.h"

#include <unordered_map>
#include <queue>

class Mirror
{
  private:
  DetailLoadState load_;
  std::queue<Message> mesages_;
  std::unordered_map<IP, ClassInfo> clis_data_;
  lfqueue<std::pair<IP, ClassInfo>> cookie_;
  
  muduo::net::EventLoop loop_;
  muduo::net::TcpServer server_;
  muduo::ThreadPool pool_;
};


#endif //SRC_MIRROR_MIRROR_H