#ifndef SRC_TELEMETER_H
#define SRC_TELEMETER_H

#pragma once

#include <stdint.h>
#include <cstring>

enum class LoadLevel : uint8_t
{
  untapped  = 0, //no client dispatched on this mirror
  light     = 1,  //[0% - 50%]
  medium    = 2,  //[50% - 80%]
  heavy     = 3,  //[ >= 80%]   
};


//POD type
struct DetailLoadState
{
  uint8_t mem_load, serve_load, net_load_, reserve_seg_;
};

void copy(DetailLoadState* dest, const DetailLoadState* sour)
{  memcpy(dest, sour, sizeof(DetailLoadState));  }

#ifdef SRC_CENTER_H
//center的setting, POD type
struct Setting
{
  uint8_t mir_load_record_interval_;  //记录mirror负载到缓存的间隔(按照心跳包的个数来计数), 从配置文件中读
  uint8_t mir_dblog_interval_;        //将mirror负载缓存更新到数据库(数据文件)的间隔(缓存了这么多之后打一次log), 从配置文件中读
  uint8_t mir_max_disbeat_time_;      //多少个心跳轮无心跳视为断线
  uint8_t cli_login_cache_time_;      //清空一个cli登录缓冲的时间
};
//第一字段为60表示一分钟(60个心跳包)记录一次mir的负载情况, 
//第二字段为5表示5分钟(缓存5次)输出到遥测数据库一次

const constexpr uint _permanent_thread_num_ = 3; //center的常驻线程数量

#endif

#ifdef SRC_MIRROR_H
//mirror的setting, POD type
struct Setting
{
  uint8_t mir_beat_interval_;
  uint8_t mir_report_interval_;
  uint8_t class_cache_time_;
  uint8_t class_interval_;
};

#endif

#define RESERVE_SEGMENG_INIT_VALUE 0

namespace telemeter
{
  const constexpr char* klog_file = "TelemeterLog.txt";  //临时的遥测日志文件
  
#ifdef SRC_CENTER_H
  //双缓冲配置表解决一写多读线程安全问题
  Setting _set_cache_one = {60, 5, 15, 60}; 
  Setting _set_cache_two = {60, 5, 15, 60};
  //主要使用这个指针获取当前配置, 每次读取数据到第二个指针中, 然后交换两个指针的值
  Setting* setting = &_set_cache_one;
  Setting* setting_copy = &_set_cache_two;
#endif

#ifdef SRC_MIRROR_H

  Setting _set_cache_one = {600,1,60,600,55};
  Setting _set_cache_two = {600,1,60,600,55};

  Setting* setting = &_set_cache_one;
  Setting* setting_copy = &_set_cache_two;

#endif
  
    

  
  //获取本机负载情况的函数, 需要统计空闲内存占比, cpu负载, 空闲磁盘占比
  LoadLevel getLocalLoadLevel();
  
  //int getLocalMemLoad();
  //int getLocalSevLoad();
  //int getLocalNetLoad();
}








#endif //SRC_TELEMETER_H