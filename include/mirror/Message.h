#ifndef SRC_MIRROR_MESSAGE_H
#define SRC_MIRROR_MESSAGE_H

#pragma once

#include "../Udp.hpp"

#include <string>
#include <utility>
#include <unordered_set>

//暂时以string代表message的内容
using Content = std::string;

class Message 
{
  private:
  Content content_; //消息内容
  std::unordered_set<IP> dest_; //需要收到消息的目标
};

#endif //SRC_MIRROR_MESSAGE_H
