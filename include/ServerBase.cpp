#include <ServerBase.hpp>

bool scu_time::operator=(const scu_time &rhs) {
    return this->hour == rhs.hour && this->minute == rhs.minute;
}

scu_time scu_time::operator-(const scu_time &rhs) const{
    if (*this < rhs)
        size_t lMinute = (this->hour + 24) * 60 + this->minute;
    else
        size_t lMinute = this->hour * 60 + this->minute;
    size_t rMinute = rhs.hour * 60 + rhs.minute;
    size_t nHour = (lMinute - rMinute) % 60;
    size_t nMinute = (lMinute - rMinute) - nHour * 60;
    return scu_time(nHour, nMinute);
}

bool scu_time::operator<(const scu_time &rhs) const {
    if (this->hour > rhs.hour) {
        return false;
    } else {
        if (this->minute >= rhs.minute) {
            return false;
        } else {
            return true;
        }
    }
}

bool scu_time::operator<=(const scu_time &rhs) const {
    if (this->hour > rhs.hour) {
        return false;
    } else {
        if (this->minute > rhs.minute)
            return false;
        else
            return true;
    }
}

unsigned scu_time::toSeconds() {
    return this->hour * 3600 + this->minute * 60;
}

scu_time scu_time::toScutime(const muduo::Timestamp &timestamp){
    long allSeconds = std::stol(timestamp.toString());
    int seconds = allSeconds % 86400;
    size_t hour = seconds / 3600;
    size_t minute = (seconds - hour * 3600) / 60;
    if(hour >= 16)
        hour -= 16;
    else
        hour += 8;
    return scu_time(hour, minute);
}


