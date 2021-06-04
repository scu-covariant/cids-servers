#ifndef SRC_MIRROR_H
#define SRC_MIRROR_H

#pragma once

#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <fstream>
#include <thread>

//udp使用
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


#include <muduo/base/ThreadPool.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <nanodbc/nanodbc.h>

#include <common_head>


#include "ClassMes.hpp"

using Cli::Uuid;

class Mirror final {

    struct OldHash {
        size_t msg_, course_, event_, next_event_;

        bool operator==(const OldHash &x) {
            return this->course_ == x.course_
                   && this->event_ == x.event_
                   && this->msg_ == x.event_
                   && this->next_event_ == x.next_event_;
            //最后一位暂时不做比较
        }
    }

private:

    //停用内存相关与timestamp
    /*
    std::unordered_map<Uuid, ClassMes> *wj_clis_data_ = new std::unordered_map<Uuid, ClassMes>; //uuid->info
    std::unordered_map<Uuid, ClassMes> *wj_clis_data_copy = new std::unordered_map<Uuid, ClassMes>; //uuid->info
    std::unordered_map<Uuid, ClassMes> *ja_clis_data_ = new std::unordered_map<Uuid, ClassMes>; //uuid->info
    std::unordered_map<Uuid, ClassMes> *ja_clis_data_copy = new std::unordered_map<Uuid, ClassMes>; //uuid->info

    muduo::Timestamp *wj_timestamp_ = nullptr;
    muduo::Timestamp *wj_timestamp_copy = nullptr;
    muduo::Timestamp *ja_timestamp_ = nullptr;
    muduo::Timestamp *ja_timestamp_copy = nullptr;
    */

    std::unordered_map<Cli::Uuid, CliDes> Class2Campus;
    std::unordered_map<Cli::Uuid, OldHash> cli_info_;

//    数据库连接
    nanodbc::connection conn;
//    线程池
    muduo::ThreadPool pool_;

//  数据库配置相关，仅在程序开始时使用
//    在配置文件获取
    const std::string db_dsn = "MariaDB-server";
    const std::string db_name = "root";
    const std::string db_passwd = "root";

    std::string img_path = "/home/jol";

    //配置文件
    std::vector<scu_time> wj_Time = {
            scu_time(8, 0), scu_time(8, 55), scu_time(10, 0),
            scu_time(10, 55), scu_time(14, 0), scu_time(14, 55),
            scu_time(15, 30), scu_time(16, 55), scu_time(17, 50),
            scu_time(19, 30), scu_time(20, 25), scu_time(21, 20)
    };

    std::vector<scu_time> ja_Time = {
            scu_time(8, 15), scu_time(9, 10), scu_time(10, 15),
            scu_time(11, 10), scu_time(13, 50), scu_time(14, 45),
            scu_time(15, 40), scu_time(16, 45), scu_time(17, 40),
            scu_time(19, 20), scu_time(20, 15), scu_time(21, 10)
    };

private:
    //mirror start:

//Center相关

    //更新load情况  Task2,udp信息
    void update_data_info();

//    自我更新相关
    //更新数据库 Task3
    //暂停该函数
    /*
    void update_clis_data(); //定时更新wj_clis_data 和 ja_clis_data
    */

    //定时读取配置文件, 交换主副setting指针, 设置下次定时读取的任务 Task4
    void read_config();

    //定时清空IP信息的算法, 例如每晚十二点, Task5
    void clear_cli_data();

    //定时  更新图片与内存信息
    void update_pictures_info();

    //Client相关
    void listen_cli_beat();


    //回调查询信息
    static void on_message(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *msg,
                           muduo::Timestamp time);


    static void on_disconnected();

    void update_timestamp(const muduo::Timestamp &dataStamp, muduo::Timestamp *origin_campus_stamp,
                          muduo::Timestamp *copy_campus_stamp);

//配置文件,初始化等
    void init();

public:

    //start开始读取配置文件
    void start();

    Mirror() = default;
};

#endif //SRC_MIRROR_H