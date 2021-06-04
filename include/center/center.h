#ifndef SRC_CENTER_H
#define SRC_CENTER_H

#include "netinet/in.h"
#include "sys/socket.h"
#include "arpa/inet.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <array>
#include <list>
#include <atomic>
#include <stdexcept>
#include <thread>

#include <muduo/base/ThreadPool.h>

#include <common_head>

#include "MirDescript.h"

class center
{
private:
    std::set<uuid> all_cli_;  //查找频繁, 不从mirs_data中遍历, 额外保存
    std::unordered_map<IP, MirDescript> mirs_data_; //记录信息的树形结构

    std::atomic<bool> atom_mutex_; //读写cookie的原子锁
    std::list<uuid> cookie_;  //记录最近登陆过的cli缓冲

    muduo::ThreadPool pool_;  //线程池

private:
    //读取配置文件, 交换主副setting指针, 设置下次定时读取的任务
    void read_config();

    //开启监听cli登录的udp线程, 每隔几十次登录调用一次负载均衡分配算法更改当前的current_available_mir
    void wait_cli_login();

    //开启收听mir心跳的udp线程, mir心跳断线的算法在这里实现, 记录最近一段时间遥测信息的算法也在这里,
    void listen_mir_beat();

    //定时清空登录信息, 默认时间每晚十二点
    void clear_mirs_data();

    //负载均衡算法, 返回当前能用的mir
    IP  load_balance();

public:
    center()
            : all_cli_(),
              mirs_data_(),
              atom_mutex_(false),
              pool_() {};

    center(const center& ) = delete;

    void start();

    //崩溃时输出程序中当前全部信息到log中
    void log_info(std::exception& e, const char* crush_file);
};

#endif //SRC_CENTER_H