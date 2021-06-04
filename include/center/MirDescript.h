#ifndef SRC_CENTER_MIRDESPCRIPT_H
#define SRC_CENTER_MIRDESPCRIPT_H

#pragma once

#include <unordered_set>
#include <vector>

#include <boost/any.hpp>

#include <common_head>


using uuid = size_t;

class MirDescript
{
private:
    uint8_t beat_;  //心跳次数计数, 用于
    LoadLevel load_level_; //粗略描述负载情况
    DetailLoadState load_state_; //详细负载情况todo: memcpy(this_state, mir_descript.load_state_)
    std::unordered_set<uuid> cli_ids_;  //当前负责的cli_ip为了O(1)查找
    std::vector<DetailLoadState> load_cache_; //记录过去一段时间mir负载的缓冲

public:
    static const constexpr int noexist = -1;

public:
    //mirdescrip只可能被默认创建后进行修改, 因此无其他构造函数
    MirDescript()
            : beat_(telemeter::setting->mir_max_disbeat_time_),
              load_level_(LoadLevel::untapped),
              load_state_({0, 0, 0, RESERVE_SEGMENG_INIT_VALUE}),
              cli_ids_(),
              load_cache_() {}

    MirDescript(const MirDescript& md)
            : beat_(md.beat_),
              load_level_(md.load_level_),
              load_state_(md.load_state_),
              cli_ids_(md.cli_ids_),
              load_cache_(md.load_cache_) {}

public:
    uint decre_and_get_beat() { beat_--; return static_cast<uint>(beat_);  }

    void reset_beat(uint max_disbeat)  { assert(max_disbeat <= 128), this->beat_ = max_disbeat;  }

    LoadLevel get_load_level() const  { return this->load_level_; }

    void set_load_level(LoadLevel ll) { this->load_level_ = ll; }

    void set_detail_state(DetailLoadState&& load) { memcpy(&(this->load_state_), &load, sizeof(DetailLoadState)); }

    bool contains(uuid cli_id) { return this->cli_ids_.find(cli_id) == std::end(this->cli_ids_) ? false : true;  }

    void erase_cli(uuid cli_id)  { this->cli_ids_.erase(cli_id); }

    std::vector<uuid> get_dispatched_cli() {
        std::vector<uuid> ret {};
        ret.reserve(cli_ids_.size());
        for(auto cli_id : cli_ids_)
            ret.push_back(cli_id);
        return ret;
    }
};


#endif //SRC_CENTER_MIRDESPCRIPT_H