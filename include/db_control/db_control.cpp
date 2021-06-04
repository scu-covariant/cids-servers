#pragma once

#include <nanodbc/nanodbc.h>
#include "../mirror/lesson.hpp"
#include "../ServerBase.hpp"
#include <map>
#include "../mirror/ClassMes.hpp"
#include <ServerBase.hpp>
#include <unordered_map>

namespace db_control {

    void check_week_and_day(nanodbc::connection conn, scu_date &date) {
        if (date.day == 7) {
            date.week += 1;
            date.day = 1;
        } else {
            date.day += 1;
        }
    }

    /*
     * param CCmap uuid->ClassMes
     * param skjc 上课节次
     *
     */

    void
    query_lessons(nanodbc::connection conn, size_t skjc, bool two_lesson, std::unordered_map<Uuid, ClassMes> &CCmap,
                  scu_date *date, Uuid id = -1) {
        size_t week = date->week; //第几周
        size_t day = date->day; //星期几
        bool new_client = false;
        std::string find_stmt{};

        std::string uuid_stmt{};

        if (id < 0) {
            auto iter = CCmap.begin();
            uuid_stmt += "( U.uuid = " + std::to_string(iter->first);
            for (auto iter = CCmap.begin(); iter != CCmap.end(); iter++) {
                uuid_stmt += " OR\n U.uuid = ";
                uuid_stmt += std::to_string(iter->first);
            }
            uuid_stmt += ") AND\n"
        } else {
            uuid_stmt = std::string("U.uuid = " + std::to_string(id));
            new_client = true;
        }
        auto get_find_stmt = [&](size_t skJc) {
            find_stmt = std::string("SELECT\n"
                                    "U.uuid AS uuid,\n"
                                    "P.kch AS `kch`,\n"
                                    "P.kxh AS `kxh`,\n"
                                    "K.kcm AS `kcm`,\n"
                                    "P.jsxm AS `jsxm`,\n"
                                    "P.jxdd AS `jdxx`,\n"
                                    "p.jsszxqh AS `jsszxqh`\n"
                                    "FROM\n"
                                    "uuid_jxdd AS U\n"
                                    "INNER JOIN pksj AS P ON U.jxdd = P.jxdd AND U.jsszxqh = P.jsszxqh\n"
                                    "INNER JOIN kcb AS K ON P.kch = K.kch\n"
                                    "WHERE\n"
                                    + uuid_stmt.c_str() +
                                    "P.sksj <= " + skJc + " AND\n"
                                                          "P.sksj+P.cxjc\t >= " + skJc + " AND\n"
                                                                                         "P.skxq = " + day + " AND\n"
                                                                                                             "SUBSTR(qsz," +
                                    week + ",1)=\'1\'");
        };

        auto update_map = [&](size_t jc, bool first, bool new_cli = false;) {
            get_find_stmt(skjc);
            try {
                nanodbc::result find_row = nanodbc::execute(conn, find_stmt);
                for (; find_row.next();) {
                    int uid = find_row.get<int>(0);

                    if (new_cli) {
                        int xqh = find_row.get<int>(6);
                        std::string dd = find_row.get<std::string>(5);
                        ClassPlace cp;
                        cp.jxdd_ = dd;
                        cp.jsszxqh = xqh;
                        ClassMes ci;
                        ci.classPlace_ = cp;
                        CCmap.insert(std::make_pair(id, ci));
                    }

                    if (CCmap.find(uid) != CCmap.end()) {
                        lesson les = lesson(find_row.get<std::string>(1), find_row.get<int>(2),
                                            find_row.get<std::string>(3), find_row.get<std::string>(4),
                                            find_row.get<std::string>(5));
                        if (first)
                            CCmap.at(uid).lesson_1 = les;
                        else
                            CCmap.at(uid).lesson_2 = les;
                    } else {
                        continue;
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl; //更换为日志报错
            }
        };

//            更新当前节次
        update_map(skjc, true, new_client);
        if (two_lesson) {
//                更新下一节次
            update_map(skjc + 1, false, new_client);
        }

    }
