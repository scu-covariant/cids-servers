#include <center_head>

void center::start()
{
    //任务队列最大数量
    pool_.setMaxQueueSize(30);
    //用std::hard_ware_concurrency()确定硬件支持的最大并发数量, 该数量 * 2为此程序运行时占用线程数
    pool_.start(static_cast<int>(2 * std::thread::hardware_concurrency()));
    auto thd_start = [&](const char* info){
        std::clog << "thread start: " << info << std::endl;
    };
    //提交定时读取配置文件的任务,监听cli登录, mir心跳的任务
    this->pool_.run( [this](){  read_config();  });
    thd_start("read config");
    this->pool_.run( [this](){  wait_cli_login();   });
    thd_start("wait cli login");
    this->pool_.run( [this](){  listen_mir_beat();  });
    thd_start("listen mir beat");
    this->pool_.run( [this](){  clear_mirs_data();  });
    thd_start("clear mir data");

    //设置原子锁的值
    this->atom_mutex_ = false;

    //日志输出的文件和文件路径在telemeter命名空间下

}

void center::read_config()
{
    static std::string config_file_path {"CenterConfig.txt"};
    static std::ifstream in {config_file_path, std::ios::in};
    Setting set {}; memset(&set, 0, sizeof(Setting));
    uint next_read_interval {15};

    auto read_data = [&]() {
        if(!in.is_open())
            in.open(config_file_path, std::ios::in);
        std::string line {};
        while (in.good())
        {
            std::getline(in, line, '\n');
            if(line.size() < 2 || line.front() == '/')
                continue;
            else if(line.find("next_read") != std::string::npos)
                next_read_interval = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1,line.begin() +  line.find_first_of(']')));
            else if(line.find("load_record") != std::string::npos)
                set.mir_load_record_interval_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1,line.begin() +  line.find_first_of(']')));
            else if(line.find("load_dblog") != std::string::npos)
                set.mir_dblog_interval_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1, line.begin() + line.find_first_of(']')));
            else if(line.find("max_disbeat") != std::string::npos)
                set.mir_max_disbeat_time_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1,line.begin() +  line.find_first_of(']')));
            else if(line.find("login_cache") != std::string::npos)
                set.cli_login_cache_time_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1, line.begin() + line.find_first_of(']')));
            else if(line.find("balance") != std::string::npos)
                set.load_balance_interval_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1, line.begin() + line.find_first_of(']')));
            else if(line.find("clear_mirs") != std::string::npos)
                set.clear_mirs_data_time_ = std::stoi(std::string(line.begin() + line.find_first_of('[') + 1, line.begin() + line.find_first_of(']')));
        }
    };

    auto swap = [](Setting* src, Setting* dst){
        Setting* tmp = dst;
        dst = src;
        src = tmp;
    };

    while(true)
    {
        //读取配置文件
        read_data();

        //复制到配置副本中
        memcpy(telemeter::setting_copy, &set, sizeof(Setting));

        //交换主副配置指针
        swap(telemeter::setting_copy, telemeter::setting);  //atomic operation

        LOG_INFO << "read config, interval: " << next_read_interval  << "s, current setting:";
        LOG_INFO << "load record interval: " << telemeter::setting->mir_load_record_interval_;
        LOG_INFO << "load database interval: " << telemeter::setting->mir_dblog_interval_;
        LOG_INFO << "load balance interval: " << telemeter::setting->load_balance_interval_;
        LOG_INFO << "max disbeat interval: " << telemeter::setting->mir_max_disbeat_time_;
        LOG_INFO << "clear mirs data time: " << telemeter::setting->clear_mirs_data_time_;
        LOG_INFO << "login cache time: " << telemeter::setting->cli_login_cache_time_;
        //挂起该进程
        sleep(next_read_interval);
    }
}

void center::wait_cli_login()
{
    static IP available_mir {0};
    static size_t login_count {0};
    auto load_balance_warpper = [&]()
    {
        available_mir = this->load_balance();
    };

    //纠正树形结构中cli信息的任务, 将交给线程池异步执行
    auto correct_tree_structure = [&](uuid cli_id){
        LOG_INFO << "correct info in tree-like structure of client id: " << cli_id;
        for(auto& kvp : mirs_data_)
        {
            if(kvp.second.contains(cli_id))
            {
                kvp.second.erase_cli(cli_id);
            }
        }

    };

    //用于接收数据的缓冲区
    std::array<char, protocol::kCLI_LOGIN_PACSIZE_> packet {0};
    std::array<char, protocol::kCLI_LOGIN_UID_> uidbuf {0};
    std::array<unsigned char, protocol::kCLI_LOGIN_STATE_> statebuf {0};
    //创建socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    //服务端addr和客户端addr, 后者保存发送方的信息
    struct sockaddr_in addr_serv, addr_cli;

    //初始化addr_serv, 无需初始化addr_cli
    memset(&addr_serv, 0, sizeof(sockaddr_in));
    memset(&addr_cli, 0, sizeof(sockaddr_in));
    addr_serv.sin_family = AF_INET;                       //使用IPV4地址
    addr_serv.sin_port = htons(static_cast<uint16_t>(port::kCLI_LOGIN_));        //端口
    /* INADDR_ANY表示不管是哪个网卡接收到数据，只要目的端口是SERV_PORT，就会被该应用程序接收到 */
    //int n = inet_aton("172.24.153.237", &addr_serv.sin_addr);
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);        //自动获取IP地址
    unsigned int place_holder_1 = sizeof(addr_cli);
    //const int place_holder_2 = 1;

    //进入循环前统一进行异常检测
    if(sock_fd < 0)
        throw std::runtime_error("Init socket failed in port::kCLI_LOGIN_");
    //解决bind绑定失败的遗留
    //setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&place_holder_2,sizeof(place_holder_2));
    int ret = bind(sock_fd, (sockaddr*)&addr_serv, sizeof(addr_serv));
    if(ret < 0)
        throw std::runtime_error("Bind socket failed in port::kCLI_LOGIN_");

    //线程主循环
    while(true)
    {
        char recv_buf[8];
        memset(recv_buf, 0, sizeof(recv_buf));
        int recv_num = recvfrom(sock_fd,recv_buf, protocol::kCLI_LOGIN_PACSIZE_, MSG_WAITALL, (sockaddr*)&addr_cli, &place_holder_1);
        //MSG_WAITALL：要求阻塞操作，直到请求得到完整的满足。
        //如果捕捉到信号，错误或者连接断开发生，或者下次被接收的数据类型不同，仍会返回少于请求量的数据。

        //可能是阻塞超时, 进行下次循环
        if(recv_num < 0){
            LOG_WARN << "recv from client error";
            continue;
        }

        for(int i = 0; i < 8; i++)
            packet[i] = recv_buf[i];
        //按照协议格式进行解析, 并且得到ip等其他信息
        memcpy(uidbuf.data(), packet.data(), protocol::kCLI_LOGIN_UID_);
        for(auto& ch :uidbuf)
            ch += '0';
        size_t uid = std::atol(uidbuf.data());
        memcpy(statebuf.data(), packet.data() + protocol::kCLI_LOGIN_UID_, protocol::kCLI_LOGIN_STATE_);
        size_t state = statebuf[0];
        //state暂时不做处理

        //是否在cookie中找到登陆记录
        bool cached {false};

        //登录或者重新连接
        if(!this->cookie_.empty())
        {
            while(true)
            {
                //空闲状态可以使用
                if(atom_mutex_ == false)
                {
                    //进入临界区
                    atom_mutex_ = true;
                    for(uuid cache : cookie_)
                    {
                        //找到登陆痕迹直接掠过
                        if(cache == uid)
                        {
                            cached = true;
                            break;
                        }
                    }
                    //结束临界区
                    atom_mutex_ = false;
                    break;
                }
                else //锁被pop队头或者push任务的任务使用中
                {
                    continue;
                }
            }
            if(cached == true)
            {
                //此处应当分析包中state来统计网络不畅的情况, 暂时忽略
                continue;
            }
        }

        //cookie为空或者没有找到时, 记录cli登录信息
        if(cached == false)
        {
            if(this->mirs_data_.size() == 0)
                memset(&available_mir, 0, sizeof(IP));
            else if(this->mirs_data_.size() == 1)
                available_mir = (*mirs_data_.begin()).first;
            //回复可用mir地址
            char send_buf[4] = {available_mir.seg0,available_mir.seg1,available_mir.seg2,available_mir.seg3};
            int send_num = sendto(sock_fd, send_buf, protocol::kCENT_RESPONSE_PACSIZE_, 0, (sockaddr*)&addr_cli, sizeof(addr_cli));
            login_count++;

            //数据库记录日志
            //dblog(cli_login, uid);
            LOG_INFO << "client login, uid: " << uid << ", reply mirs ip: " << available_mir.to_string();
            if(this->all_cli_.count(uid) == 0)
                this->all_cli_.insert(uid);
            else
                this->pool_.run(std::bind(correct_tree_structure, uid));
            while(true)
            {
                //空闲状态可以使用
                if(atom_mutex_ == false)
                {
                    //进入临界区
                    atom_mutex_ = true;
                    //这个push操作似乎不需要上锁
                    cookie_.push_back(uid);
                    //结束临界区
                    atom_mutex_ = false;

                    //设置一段时间后pop掉队头的任务
                    this->pool_.run( [this, &uid](){
                        sleep(telemeter::setting->cli_login_cache_time_);
                        while(true)
                        {
                            if(atom_mutex_ == false)
                            {
                                atom_mutex_ = true;
                                uuid id = cookie_.front();
                                cookie_.pop_front();
                                atom_mutex_ = false;
                                LOG_INFO << "erase login cache of client: " << id;
                            }
                            else{
                                LOG_INFO << "cached client login, no reply for client: " << uid;
                                continue;
                            }

                        }
                    });
                    break;
                }
                else //锁被pop队头或者查询任务使用中
                {
                    LOG_INFO << "cached client login, no reply for client :  " << uid;
                    continue;
                }
            }
        }
        //重置输入缓冲区
        packet.fill(0);

        if(login_count >= telemeter::setting->load_balance_interval_)
        {
            //提交给线程池而不阻塞在io线程, 对available_mir的修改是跨线程操作变量
            this->pool_.run(load_balance_warpper);
            login_count = 0;
        }
    }
}

void center::listen_mir_beat()
{
    IP mir_ip {0};  //心跳的mir ip
    size_t all_beat_count {0}; //用于判活算法

    //用于接收数据的缓冲区
    std::array<uint8_t, protocol::kMIR_BEAT_PACSIZE_> packet {0};
    std::array<uint8_t, protocol::kMIR_BEAT_IP_> ipbuf {0};
    std::array<uint8_t, protocol::kMIR_BEAT_LOAD_> loadbuf {0};
    //创建socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    //服务端addr和客户端addr, 后者保存发送方的信息
    struct sockaddr_in addr_serv, addr_mir;

    //初始化addr_serv, 无需初始化addr_cli
    memset(&addr_serv, 0, sizeof(sockaddr_in));
    memset(&addr_mir, 0, sizeof(sockaddr_in));
    addr_serv.sin_family = AF_INET;                       //使用IPV4地址
    addr_serv.sin_port = htons(port::kMIR_BEAT_);        //端口
    /* INADDR_ANY表示不管是哪个网卡接收到数据，只要目的端口是SERV_PORT，就会被该应用程序接收到 */
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);        //自动获取IP地址
    unsigned int place_holder_1 = sizeof(addr_mir);

    //进入循环前统一进行异常检测
    if(sock_fd < 0)
        throw std::runtime_error("Init socket failed in port::kMIR_BEAT_");
    if(bind(sock_fd, (sockaddr*)&addr_serv, sizeof(addr_serv)) < 0)
        throw std::runtime_error("Bind socket failed in port::KMIR_BEAT_");


    //线程主循环
    while(true)
    {
        char recv_buf[8] = {0};
        memset(recv_buf, 0, sizeof(recv_buf));
        int recv_num = recvfrom(sock_fd, recv_buf, protocol::kMIR_BEAT_PACSIZE_, MSG_WAITALL, (sockaddr*)&addr_mir, &place_holder_1);
        //MSG_WAITALL：要求阻塞操作，直到请求得到完整的满足。
        //如果捕捉到信号，错误或者连接断开发生，或者下次被接收的数据类型不同，仍会返回少于请求量的数据。

        //可能是阻塞超时, 进行下次循环
        if(recv_num < 0) {
            LOG_WARN << "recv from mirror error";
            continue;
        }

        for(int i = 0; i < 8; i++)
            packet[i] = recv_buf[i];
        //按照协议格式进行解析, 并且得到ip等其他信息
        memcpy(ipbuf.data(), packet.data(), protocol::kMIR_BEAT_IP_);

        memcpy(loadbuf.data(), packet.data() + protocol::kMIR_BEAT_IP_, protocol::kMIR_BEAT_LOAD_);

        mir_ip = IP(ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);
        LOG_INFO << mir_ip.to_string() << " send packet " << packet.data();

        if(mirs_data_.count(mir_ip) == 0)
        {
            mirs_data_.insert({mir_ip, MirDescript()});
            //dblog(MIR_LIGIN);
        }
        else
        {
            mirs_data_[mir_ip].reset_beat(telemeter::setting->mir_max_disbeat_time_);
            //记录负载情况到缓存
            //mirs_data_[mir_ip].recordLoad();
            all_beat_count++;
            if(all_beat_count >= mirs_data_.size() - 1)
            {
//                for(auto& kvp : mirs_data_)
//                {
//                    if(kvp.second.decre_and_get_beat() <= 0);
//                    {
//                        //说明有mir掉线了
//                        int i = 3;  //占位避免编译器报warning
//                        //dblog(MIR_DISCONECT);
//                        mirs_data_.erase(kvp.first);
//                        LOG_INFO << "mirror disconnect: " << const_cast<IP*>(&kvp.first)->to_string();
//                    }
//                }
            }
        }

    }
}

void center::clear_mirs_data()
{
    auto get_next_zeropoint = [](){
        time_t t = time(NULL);
        struct tm * tm= localtime(&t);
        tm->tm_mday += 1;
        tm->tm_hour = 0;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        return mktime(tm);
    };

    while(true)
    {
        sleep(get_next_zeropoint() - time(NULL) + telemeter::setting->clear_mirs_data_time_);
        mirs_data_.clear();
        all_cli_.clear();
        LOG_INFO << "clear mirrors data successfully";
    }
}

IP center::load_balance()
{
    LOG_INFO << "load balance occurs";
    //暂时不优化
    for(auto& kvp : mirs_data_)
    {
        if(kvp.second.get_load_level() == LoadLevel::light)
            return kvp.first;
    }

    for(auto& kvp : mirs_data_)
    {
        if(kvp.second.get_load_level() == LoadLevel::medium)
            return kvp.first;
    }

    for(auto& kvp : mirs_data_)
    {
        if(kvp.second.get_load_level() == LoadLevel::untapped)
        {
            kvp.second.set_load_level(LoadLevel::light);
            return kvp.first;
        }
    }

    for(auto& kvp : mirs_data_)
    {
        if(kvp.second.get_load_level() == LoadLevel::heavy)
        {
            return kvp.first;
        }
    }

    //没有任何一个可用的服务器
    throw std::runtime_error("No available mir to dispatch");
}

void center::log_info(std::exception& e, const char* crush_file)
{
    //以后要改到json里面, 还缺详细个数的输出
    static std::ofstream log(crush_file, std::ios::out);
    if(!log.is_open())
        log.open(crush_file, std::ios::out);
    log <<"crush time: " << time(nullptr) <<"\nreason: " << e.what() << std::endl;

    log << "Mirrors & dispatched client:\n";
    for(auto& kvp : mirs_data_)
    {
        log << "ip: " << const_cast<IP*>(&kvp.first)->to_string()
            << " load state: " << tostring(kvp.second.get_load_level()) << '\n';
        log << "dispatched client: \n";
        for(auto uid : kvp.second.get_dispatched_cli())
        {
            log << uid << '\n';
        }
    }

    log << "cookie mutex: " << atom_mutex_ << '\n';
    if(cookie_.size() > 0)
    {
        log << "cookie :\n";
        for(auto uid : cookie_)
        {
            log << uid << '\n';
        }
    }
}
