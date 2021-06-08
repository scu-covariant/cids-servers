//
// Created by XM on 2021/5/18.
//

#ifndef PRO_SERVERBASE_HPP
#define PRO_SERVERBASE_HPP

#pragma once

#include <muduo/base/Timestamp.h>

struct scu_date {
    int week;
    int day;
    scu_date(int w, int d): week(w), day(d) { };
};

class scu_time {

private:
    size_t hour;
    size_t minute;

public:

    scu_time(size_t h, size_t m) : hour(h), minute(m) {}

    scu_time(const scu_time &that) : hour(that.hour), minute(that.minute) {}

    scu_time operator-(const scu_time &rhs) const;

    bool operator<(const scu_time& rhs) const;

    bool operator=(const scu_time &rhs);

    bool operator<=(const scu_time &rhs) const;

    unsigned toSeconds();

    static scu_time toScutime(const muduo::Timestamp &timestamp) ;


};


#endif //PRO_SERVERBASE_HPP
