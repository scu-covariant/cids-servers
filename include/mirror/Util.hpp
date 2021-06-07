#pragma once

#include <string>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/reader.h>

#include <muduo/base/Timestamp.h>

#include <unordered_map>
#include <map>
#include <nanodbc/nanodbc.h>

#include <mirror_head>
#include <common_head>


namespace db_control {
    using Cli::Uuid;

    void check_week_and_day(nanodbc::connection conn, scu_date &date);

    /*
     * param CCmap uuid->ClassMes
     * param skjc 上课节次
     *
     */
    void
    query_lessons(nanodbc::connection conn, size_t skjc, bool two_lesson, std::unordered_map<Uuid, ClassMes> &CCmap,
                  scu_date *date, Uuid id = -1);


    void query_background(nanodbc::connection conn, std::vector<backgrounds>& backgrounds);

    void update_pic_info(nanodbc::connection conn, wj_clis_cache, const std::string &stamp);

    std::string find_pic(const nanodbc::connection &conn, Cli::Uuid id, const std::string &stamp);

    std::deque<Message> find_mes(const nanodbc::connection &conn, Cli::Uuid id, const std::string stamp);

    Lesson find_lesson(const nanodbc::connection &conn, Cli::Uuid, const std::string& stamp);
}


namespace json_control {

    void getJson(rapidjson::Writer <rapidjson::StringBuffer> &writer, const Message &mes);

    void getJson(rapidjson::Writer <rapidjson::StringBuffer> &writer, const Lesson &les);

    std::string getJsonInfo(const ClassMes &mes, const muduo::Timestamp &timestamp);

    void getIdAndStamp(const std::string& src, Cli::Uuid &id, std::string& stamp);
}//End namespace of json_control

namespace codec{
    void downPic( const std::string &code,const std::string& path);

}