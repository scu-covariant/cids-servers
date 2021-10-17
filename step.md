* 切换为root

sudo -s
(**you passwd**)

* 基本工具
apt update  
apt upgrade  
apt install g++  
apt install cmake  
apt install make  
apt install git  

* 准备环境
cd home  
mkdir server  
cd server  

* 准备第三方库

  # 下载boost库
tar -jxvpf boost_1_74_0.tar.bz2  
cd boost_1_74_0  
./bootstrap.sh --with-libraries=all --with-toolset=gcc  
./b2 install  


cd /home/server
# muduo库

git clone https://github.com/chenshuo/muduo.git  
cd muduo  
mkdir build  
cd build  
cmake ..  
make -12  

# 准备lib里的内容


cd /home/server  
# rapidjson库  

git clone https://github.com/Tencent/rapidjson.git
cd rapidjson  
mkdir build 
cd build  
cmake ..  
make -j12  
make install  

# 准备包(package)

cd /home/server
# nanodbc库

# 准备mariadb(已准备)

# 下载unixodbc
apt install unixodbc

#下载odbc_config
tar -xzvf  unixODBC-2.3.9.tar.gz  
cd unixODBC-2.3.9
./configure --enable-gui=no  
make  
sudo make install  
odbc_config  --version  #check odbc_config is installed  

# 下载odbc_mariadb
apt install odbc-mariadb


# 配置odbc_mariadb
### configure odbc-mariadb

see complete info on "https://mariadb.com/kb/en/creating-a-data-source-with-mariadb-connectorodbc/"

[MariaDB ODBC 3.0 Driver]
Description = MariaDB Connector/ODBC v.3.0  
Driver=/usr/lib/x86_64-linux-gnu/odbc/libmaodbc.so  


cd /home/server  
# 下载nanodbc库
git clone https://github.com/nanodbc/nanodbc.git
cd nanodbc  
mkdir build  
cd build  
cmake ..  
make -j12  
make install  

cd /home/server
# 准备好 
cp ./muduo/build/lib/*.a ./cids-pack/Lib/

cd /cids-pack  
mkdir build  
cmake ..  
make -j12  

