#pragma once

#include "../Udp.hpp"

#include <string>
#include <utility>
#include <unordered_set>

//暂时以string代表message的内容
using Content = std::string;

struct Content final{
    std::string time_;
    std::string image_url;
    std::string message_;
    std::string event_;
};

class Message 
{
  private:
  Content content_; //消息内容
  std::unordered_set<uint8_t> dest_; //需要收到消息的目标
};
