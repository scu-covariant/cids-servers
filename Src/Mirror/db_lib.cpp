#include <mirror_head>
#include <common_head>

namespace db_control {

    using Cli::Uuid;

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
    query_lessons(nanodbc::connection conn, size_t skjc, bool two_lesson, std::unordered_map <Uuid, ClassMes> &CCmap,
                  scu_date *date, Uuid id) {
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
            uuid_stmt += ") AND\n";
        } else {
            uuid_stmt = std::string("U.uuid = " + std::to_string(id));
            new_client = true;
        }
        auto get_find_stmt = [&](size_t skJc) {
            find_stmt = std::string(std::string("SELECT\n"
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
                                                "WHERE\n")
                                    + uuid_stmt +
                                    "P.sksj <= " + std::to_string(skJc) + " AND\n"
                                                                          "P.sksj+P.cxjc\t >= " + std::to_string(skJc) +
                                    " AND\n" +
                                    "P.skxq = " + std::to_string(day) + " AND\n"
                                    + "SUBSTR(qsz," +
                                    std::to_string(week) + ",1)=\'1\'");
        };

        auto update_map = [&](size_t jc, bool first, bool new_cli = false) {
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
                        Lesson les = Lesson(find_row.get<std::string>(1), find_row.get<int>(2),
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

    void query_background(nanodbc::connection conn, std::vector <backgrounds> &backgrounds) {
        //查询所有
        std::string sel_stmt = "SELECT * FROM backgrounds";
        //执行语句
        nanodbc::result find_row = nanodbc::execute(conn, sel_stmt);

        for (; sel_stmt.next();) {
            int id = sel_stmt.get<int>(0);
            std::string code = sel_stmt.get<std::string>(1);
            std::string des = sel_stmt.get<std::string>(2);

            backgrounds.emplace_back(id, code, des);
        }
    }

    std::string find_pic(const nanodbc::connection &conn, Cli::Uuid id) {
        nanodbc::statement statement(conn);

        nanodbc::prepare(statement, std::string("SELECT background FROM places WHERE UUID = ?"));

        statement.bind(0, std::to_string(id));

        nanodbc::result row = nanodbc.execute(statement);

        while (row.next()) {
            return row.get<std::string>(0);
        }
    }


     Lesson find_event(const nanodbc::connection &conn, Cli::Uuid id, nanodbc::connection conn, size_t skjc, scu_date *date, Uuid id){

    }

    std::deque<Message> find_message(const nanodbc::connection& conn, Cli::Uuid id, muduo::Timestamp &stamp){

    }



}
