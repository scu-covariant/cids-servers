#ifndef SRC_CENTER_H
#define SRC_CENTER_H

#include "muduo/base/ThreadPool.h"

#include "Udp.hpp"
#include "Telemeter.hpp"
#include "../Type.hpp" //lock free queue -> lfqueue

#include "MirDescript.h"

#include <unordered_map>

class center
{
private:
  //O(1)查找所有可用的mirip地址, [不再]初始化时从db中取出预存, [而是]从心跳包中分析即可, 心跳包最后一段表示当前连接数[处理mirror连接情况与center缓存数据不一致的问题]
  //记录树形mir->cli信息的数据结构, list? hash_table?, 需要缓存mir负载信息以供调度算法使用?
  //一种变量, 记录过去一段时间每个mir的平均负载情况, 定时更新到遥测数据库里 
  using uuid = size_t;
  std::set<uuid> all_cli_;  //查找频繁, 不从mirs_data中遍历, 额外保存
  std::unordered_map<IP, MirDescript> mirs_data_; //记录信息的树形结构
  lfqueue<IP> cookie_;  //记录最近登陆过的cli缓冲
  muduo::ThreadPool pool_;  //线程池

private:
  //center start:

  //读取配置文件, 交换主副setting指针, 设置下次定时读取的任务
  void read_config();

  //开启监听cli登录的udp线程, 每隔几十次登录调用一次负载均衡分配算法更改当前的current_available_mir
  void wait_cli_login();

  //开启收听mir心跳的udp线程, mir心跳断线的算法在这里实现, 记录最近一段时间遥测信息的算法也在这里,
  void listen_mir_beat();

  //定时清空登录信息的算法, 例如每晚十二点, 
  void clear_mirs_data();

public:
  
  void start();
};

#endif //SRC_CENTER_H