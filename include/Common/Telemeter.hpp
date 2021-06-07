#ifndef SRC_TELEMETER_H
#define SRC_TELEMETER_H

#pragma once

#include <fmt/core.h>

#include <stdint.h>
#include <cstring>

enum class LoadLevel : uint8_t {
    untapped = 0, //no client dispatched on this mirror
    light = 1,  //[0% - 50%]
    medium = 2,  //[50% - 80%]
    heavy = 3,  //[ >= 80%]
};

static std::string tostring(LoadLevel ll) {
    switch (ll) {
        case LoadLevel::untapped:
            return "untapped";
        case LoadLevel::light:
            return "light";
        case LoadLevel::medium:
            return "medium";
        case LoadLevel::heavy:
            return "heavy";
        default:
            return "undefined";
    }
}


//POD type
struct DetailLoadState {
    uint8_t mem_load, serve_load, net_load_, reserve_seg_;
};

#ifdef SRC_CENTER_H
//center的setting, POD type
struct Setting
{
  uint mir_load_record_interval_;  //记录mirror负载到缓存的间隔(按照心跳包的个数来计数), 从配置文件中读
  uint mir_dblog_interval_;        //将mirror负载缓存更新到数据库(数据文件)的间隔(缓存了这么多之后打一次log), 从配置文件中读
  uint mir_max_disbeat_time_;      //多少个心跳轮无心跳视为断线
  uint cli_login_cache_time_;      //清空一个cli登录缓冲的时间
  uint load_balance_interval_;     //多少个cli启动后运行一次负载均衡调度算法
  uint clear_mirs_data_time_;      //自0点开始何时清空一次mir_data中的信息
  //unsigned long reserve_seg_;    //保留段
};
//第一字段为60表示一分钟(60个心跳包)记录一次mir的负载情况,
//第二字段为5表示5分钟(缓存5次)输出到遥测数据库一次

const constexpr uint _permanent_thread_num_ = 4U; //center的常驻线程数量

#endif

#ifdef SRC_MIRROR_H
//mirror的setting, POD type
struct Setting
{
  std::string center_ip_; //?解析
//  int backup_center_ip_;
  int next_read_interval_;
  uint8_t mir_heart_beat_interval_;
  uint16_t mir_load_report_interval_;
  uint16_t cli_class_info_cache_time_;
  //数据库ip?
};
#endif

#define RESERVE_SEGMENG_INIT_VALUE 0U

namespace telemeter {
    const constexpr char *klog_file = "TelemeterLog.txt";  //临时的遥测日志文件

#ifdef SRC_CENTER_H
    //双缓冲配置表解决一写多读线程安全问题

  static Setting _set_cache_one = {60, 5, 15, 60, 20, 0};
  static Setting _set_cache_two = {60, 5, 15, 60, 20, 0};
  //主要使用这个指针获取当前配置, 每次读取数据到第二个指针中, 然后交换两个指针的值
  static Setting* setting = &_set_cache_one;
  static Setting* setting_copy = &_set_cache_two;

  //将配置信息返回成一个字符串输出
  // static auto settting_info = [](Setting* set = telemeter::setting) -> std::string
  // {
  //   return fmt::format("mir load record interval:   -{0}\n \
  //   nmir database log interval:  -{1}\n \
  //   mir max disbeat time:       -{2}\n  \
  //   cli login cache time:       -{3}\n  \
  //   load balance interval:      -{4}\n  \
  //   clear mirs data time:       -{5}",
  //   set->mir_load_record_interval_,
  //   set->mir_dblog_interval_,
  //   set->mir_max_disbeat_time_,
  //   set->cli_login_cache_time_,
  //   set->load_balance_interval_,
  //   set->clear_mirs_data_time_);
  // };
#endif

#ifdef SRC_MIRROR_H
    static Setting _set_cache_one = {0,600,1,60,600};
    static Setting _set_cache_two = {0,600,1,60,600};
    //主要使用这个指针获取当前配置, 每次读取数据到第二个指针中, 然后交换两个指针的值
    static Setting* setting = &_set_cache_one;
    static Setting* setting_copy = &_set_cache_two;

#endif
    //获取本机负载情况的函数, 需要统计空闲内存占比, cpu负载, 空闲磁盘占比
    //LoadLevel getLocalLoadLevel();

    //int getLocalMemLoad();
    //int getLocalSevLoad();
    //int getLocalNetLoad();
}


#endif //SRC_TELEMETER_H