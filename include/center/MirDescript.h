#ifndef SRC_CENTER_MIRDESPCRIPT_H
#define SRC_CENTER_MIRDESPCRIPT_H

#pragma once

#include "Telemeter.hpp"
#include "Udp.hpp"

#include <unordered_set>
#include <boost/any.hpp>
#include <vector>

class MirDescript : public boost::noncopyable 
{
  private:
  uint8_t beat_;  //心跳次数计数, 用于
  LoadLevel load_level_; //粗略描述负载情况
  DetailLoadState load_state_; //详细负载情况todo: memcpy(this_state, mir_descript.load_state_)
  std::unordered_set<IP> cli_ips_;  //当前负责的cli_ip为了O(1)查找
  std::vector<DetailLoadState> load_cache_; //记录过去一段时间mir负载的缓冲
  
  public:
  static const constexpr int noexist = -1;

  public:
  uint getBeat() const  { return static_cast<uint>(beat_);  }
  void resetBeat(uint max_disbeat)  { assert(max_disbeat <= 128), this->beat_ = max_disbeat;  }
  LoadLevel getRoughLoad() const  { return this->load_level_; }
  void setLoadState(DetailLoadState&& load) { copy(&(this->load_state_), &load); }
  auto contains(IP cli_ip) { return auto index = this->cli_ips_.find(cli_ip), index == std::end(this->cli_ips_) ? noexist : index;  }


};






#endif //SRC_CENTER_MIRDESPCRIPT_H