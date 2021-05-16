#ifndef SRC_MIRROR_H
#define SRC_MIRROR_H

#include "muduo/base/ThreadPool.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"

#include "../Telemeter.hpp"
#include "../Type.hpp"  //lock free queue -> lfqueue

#include "ClassInfo.hpp"
#include "Message.hpp"

#include <unordered_map>
#include <queue>

class Mirror
{
  private:
  DetailLoadState load_;
  std::queue<Message> messages_;
  std::unordered_map<uint8_t , ClassInfo> clis_data_; //uuid->info
  lfqueue<std::pair<uint8_t , ClassInfo>> cookie_;
  std::map<std::string, lesson> cli_lesson_;
  nanodbc::connection conn;

  muduo::net::EventLoop loop_;
  muduo::net::TcpServer server_;
  muduo::ThreadPool pool_;

//  数据库配置相关，仅在程序开始时使用
  const std::string db_dsn = "MariaDB-server";
  const std::string db_name = "root";
  const std::string db_passwd = "root";
private:
    //mirror start:

    //定时清空IP信息的算法, 例如每晚十二点,
    void clear_cli_data();

    //读取配置文件, 交换主副setting指针, 设置下次定时读取的任务
    void read_config();

    //更新load情况
    void update_data_info();

    //开启收听mir心跳的udp线程, mir心跳断线的算法在这里实现, 记录最近一段时间遥测信息的算法也在这里,
    void listen_cli_beat();

    //等待即时消息
    void listen_mes();

    //更新数据库
    void update_clis_data();

public:
    void start();

};

#endif //SRC_MIRROR_H