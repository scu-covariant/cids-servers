#include "Mirror.hpp"


void Mirror::start() {
//任务队列最大数量
    pool_.setMaxQueueSize(30);
    //用std::hard_ware_concurrency()确定硬件支持的最大并发数量, 该数量 * 2为此程序运行时占用线程数
    pool_.start(static_cast<int>(2 * std::thread::hardware_concurrency()));
    auto thd_start = [&](const char *info) {
        std::clog << "thread start: " << info << std::endl;
    };

    //    db_config 初始化
    nanodbc::connection conn = nanodbc::connection(db_dsn, db_name, db_passwd);

    //读取配置文件
    this->pool_.run([this]() { read_config(); });
    thd_start("read config");
    //发送负载情况
    this->pool_.run([this]() { update_data_info(); });
    thd_start("update data info");
    //更新数据缓存
    this->pool_.run([this]() { update_clis_data(); });
    thd_start("updae clients data");

    //输出日志
}

void Mirror::read_config() {

//    后可以考虑使用json格式
    static std::string config_file_path{"MirrorConfig.txt"};
    static std::ifstream in{config_file_path, std::ios::in};
    Setting set{};
    memset(&set, 0, sizeof(Setting));
    size_t next_read_interval{0};

    auto read_data = [&]() {
        if (!in.is_open())
            in.open(config_file_path, std::ios::in);
        std::string line{};
        while (in.good()) {
            std::getline(in, line, '\n');
            if (line.size() < 2 || line.front() == '/')
                continue;
            else if (line.find("next_read") != std::string::npos)
                next_read_interval = std::stoi(std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("mir_heart") != std::string::npos)
                set.mir_heart_beat_interval_ = std::stoi(
                        std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("mir_load") != std::string::npos)
                set.mir_load_report_interval_ = std::stoi(std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
            else if (line.find("cli_class") != std::string::npos)
                set.cli_class_info_cache_time_ = std::stoi(
                        std::string(line.find_first_of('[') + 1, line.find_first_of(']')));
        }
    };

    auto swap = [](Setting *src, Setting *dst) {
        Setting *tmp = dst;
        dst = src;
        src = tmp;
    };

    while (true) {
        //读取配置文件
        read_data();

        //复制到配置副本中
        memcpy(telemeter::setting_copy, &set, sizeof(Setting));

        //交换主副配置指针
        swap(telemeter::setting_copy, telemeter::setting);  //atomic operation

        //挂起该进程
        sleep(next_read_interval * 1000);
    }
}

void Mirror::update_data_info() {
//    发送ip地址
    using Server::IP;
    IP center_ip{0}; //通过配置文件
    IP this_ip{0};//unknown

    DetailLoadState dtlLoadState{};
//    定义计算服务器负载情况的函数
    auto cal_ser_load = [&]() -> uint8_t {
        return 0;
    };

//    定义计算内存负载情况的函数
    auto cal_mem_load = [&]() -> uint8_t {
        std::ifstream in;
        std::string filename = "";
        return 0;
    };

//    定义计算网络负载情况的函数
    auto cal_net_load = [&]() -> uint8_t {
        return 0;
    };

    auto get_packet = [&](IP, DetailLoadState DtlLoadState) {
        center_ip;
        this_ip;
        //calculate ServerLoad
        uint8_t serverLoad = cal_ser_load();
        DtlLoadState.serve_load = serverLoad;
        //calculate MemoryLoad
        uint8_t memLoad = cal_mem_load();
        DtlLoadState.mem_load = memLoad;
        //calculate NetworkLoad
        uint8_t netLoad = cal_net_load();
        DtlLoadState.net_load_ = netLoad;

        return "12345678";
    };



    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
//    设置发送端口
    servaddr.sin_port = htons(port::kMIR_BEAT_);
//    设置发送ip
    servaddr.sin_addr.s_addr = inet_addr(center_ip);

//    发送缓存
    char *packet = "\0";

//    更新间隔
    size_t time = 60;
//    发送的息
    char send_buf[8] = {0};

//线程主循环
    while (true) {

        if (time == telemeter::setting->mir_load_report_interval_) {
            //to_string(8byte) = 4+1+1+1+1 true/false
            //更新负载情况
            send_buf = get_packet(this_ip, dtlLoadState);
            time = 1;
        }

//        发送‘心跳包’
        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
        //        重置缓冲
        memset(sendbuf, 0, sizeof(sendbuf));
        // 统计发送心跳包次数
        time++;
        //心跳包频率
        sleep(telemeter::setting->mir_heart_beat_interval * 1000);
    }
}

//待修改
void Mirror::on_message(muduo::net::TcpConnectionPtr conn,
                        std::string msg,
                        muduo::Timestamp time) {
    //比较时间戳, 相等, 无事发生
    if (equal(time, *this->wj_timestamp_) {
        //发送不需要更新的json
        conn->send(CliDescript::NO_NEED_UPDATE);
    } else    //这里的逻辑需要完善
    {
        //回复的内容
        JsonValue reply();
        //解析msg, 得到uuid
        JsonValue pack = JsonParser(msg);
        Uuid id = pack["uuid"];
        //查询有无即时消息
        if (message_.front().get_dests().contains(id));
        {
            reply["Message"] = message_.front().to_string();
        }

    }

}

//多并发
void Mirror::listen_cli_beat(muduo::net::TcpConnectionPtr &conn,
                             muduo::net::Buffer *buf,
                             muduo::Timestamp time) {
    //set server.onConnect 连接函数
    this->pool_.run(std::bind(Mirror::on_message,
                              this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3),
                              std::move(conn),
                              std::move(std::string(buf->retrieveAllAsString())),
                              std::move(time)));
}

//监听center发送的udp包
void Mirror::listen_mes() {
// udp
    while (true) {
        //server.start

        //接收到包

        //数据库拉取信息

        //更新信息
    }
}

//清除mirror->client树信息
void Mirror::clear_cli_data() {
    while (true) {
        //每日0点更新.用scu_time更新

        //sleep()24h
    }
}

//定时拉取数据库信息, 更新时间戳
void Mirror::update_clis_data() {
    //cli-classinfo缓存
    std::unordered_map<Cli::Uuid, ClassMes> clis_data;

//    校园日历
    scu_date sD{1, 1};
    scu_date sD_1{1, 1};
    scu_date *scuDate = &sD;
    scu_date *scuDate_copy = &sD_1;

    scu_date scuDate_cache{};

    scu_time nowTime{0, 0};

    auto swap_scu_date_ptr = [&]() {
        scu_date *tmp = scuDate;
        scuDate_copy = scuDate;
        scuDate = tmp;
    };

//写当日date(其实不会出现冲突.但为以后消费者模型做准备工作)
    auto update_scu_date = [&]() {
//更新当日上课信息
        db_control::check_week_and_day(conn, scuDate_cache);
//复制至副本
        memcpy(scuDate_copy, &scuDate_cache, sizeof(scu_date));
//交换指针
        swap_scu_date_ptr();
    };

//    判断时间
    auto at_middle = [&](const scu_time &lTime, const scu_time &rTime) {
        return (lTime <= nowTime) && (nowTime < rTime);
    };

    auto get_seconds_from_now = [&](const scu_time &time) {
        nowTime = scu_time::toScutime(muduo::Timestamp::now());
        return time <= nowTime ? 0 : (time - nowTime).toSeconds();
    };

    auto query_scu_time = [&](int &xqh, int &jc) -> scu_time {
//        获取迭代器,遍历时间表

        auto iter1 = this->wj_Time.begin();
        auto iter2 = this->ja_Time.begin();
//        上课节次,用于返回
        int skjc = 0;

        nowTime = scu_time::toScutime(muduo::Timestamp::now());//获取当前时间,进行分析

        for (; iter1 != wj_Time.end(); iter1++, iter2++) {
            skjc++;
            jc = skjc;
            if (skjc <= 4) {
                if (nowTime <= *iter1) {
                    xqh = 1;
                    return scu_time(0, 15);
                } else {
                    if (nowTime <= *iter2) {
                        xqh = 3;
                        if (skjc == 4)
                            return scu_time(2, 40);
                        else
                            return scu_time(0, 40);
                    } else {
                        continue;
                    }
                }
            } else {
                if (nowTime <= *iter2) {
                    xqh = 3;
                    return scu_time(0, 10);
                } else {
                    if (nowTime <= *iter1) {
                        xqh = 1;
                        if (skjc == 9) //晚餐
                            return scu_time(1, 30);
                        else if (skjc == 7)
                            return scu_time(0, 55);
                        else if (skjc == 12)
                            return scu_time(0, 0);
                        else
                            return scu_time(0, 45);
                    } else {
                        continue;
                    }
                }
            }
        }
    };

//    自动计算当前时间
    auto query_clis_data = [&, this]() {
        std::unordered_map<Cli::Uuid, ClassMes> clis_data_cache_;
        bool two_lessons = true;
        int xqh = 0;  //校区号
        int jc = 0;    //时间

        auto update_uC_map = [](const std::unordered_map<Cli::Uuid, ClassMes> &data_map,
                                std::unordered_map<Cli::Uuid, ClassMes> *origin_map,
                                std::unordered_map<Cli::Uuid, ClassMes> *copy_map) {
            //赋值
            *copy_map = data_map;
//            交换指针
            auto tmp = copy_map;
            copy_map = origin_map;
            origin_map = tmp;
        };


        //获取应当更新的校区号与节次
        scu_time wait_time = query_scu_time(xqh, jc);

        if (jc == 4 || jc == 9 || jc == 12)
            two_lessons = false;
        size_t wait_second = wait_time.toSeconds();
        if (xqh == 1) {
//       更新值
//       参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            clis_data_cache_ = *(this->wj_clis_data_);
//         参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            db_control::query_lessons(conn, jc, two_lessons, clis_data_cache_, scuDate);
//            获取当前时间戳
            muduo::Timestamp nTime = muduo::Timestamp::now();
//            更新时间戳
            update_timestamp(nTime, wj_timestamp_, wj_timestamp_copy);
            //       交换指针

            update_uC_map(clis_data_cache_, this->wj_clis_data_, this->wj_clis_data_copy);
        } else if (xqh == 3) {
//         更新值
            clis_data_cache_ = *ja_clis_data_;
//         参数: 连接参数, 这节课节次,查询两节课信息, 要查询的uuid集合
            db_control::query_lessons(conn, jc, two_lessons, clis_data_cache_, scuDate);
//       获取当前时间戳
            muduo::Timestamp nTime = muduo::Timestamp::now();
//        更新时间戳
            update_timestamp(nTime, ja_timestamp_, ja_timestamp_copy);
//       交换指针
            update_uC_map(clis_data_cache_, this->ja_clis_data_, this->ja_clis_data_copy);
        }

        return wait_second;
    };

//    mirror开机时,初始化当时时间
    update_scu_date();
    muduo::Timestamp nTime = muduo::Timestamp::now();
    nowTime = scu_time::toScutime(nTime);


//    进入任务线程阶段
    while (true) {
//        挂起. 7.55唤醒
        sleep(get_seconds_from_now(scu_time(7, 55)) * 1000);

        while (true) {
            nowTime = scu_time::toScutime(muduo::Timestamp::now());//记录当前时间
            size_t sleepSeconds = 0;
            sleepSeconds = query_clis_data();   //更新数据,在函数体内自动计算时间

            if (sleepSeconds == 0)
                break;
            //休眠至下一次更新
            sleep(sleepSeconds * 1000);
        }
        sleep(get_seconds_from_now(scu_time(24, 0)) * 1000); //沉睡至24点
//        24点更新当日上课信息
        update_scu_date();
    }

    conn.close();
}

void Mirror::update_timestamp(const muduo::Timestamp &dataStamp, muduo::Timestamp *origin_campus_stamp,
                              muduo::Timestamp *copy_campus_stamp) {

    //    赋值
    *copy_campus_stamp = dataStamp;

//    交换指针
    auto swap = [](muduo::Timestamp *origin, muduo::Timestamp *copy) {
        muduo::Timestamp *tmp = origin;
        origin = copy;
        copy = tmp;
    };
    swap(origin_campus_stamp, copy_campus_stamp);
}

