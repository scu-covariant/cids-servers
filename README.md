# cids-servers

center server ,mirror server and driver



your project path and other library project path should be like this: 



- dir  

  - cids-server  

  - rapidjson  
  - nanodbc  

  - muduo



after clone this project



## Imporve projectPath

```bash
-- mkdir lib #to find libraries 

-- mkdir include #to find headers
```

------



## Compile rapidJson

```bash
git clone <https://github.com/Tencent/rapidjson.git> #download rapidjson

cd rapidjson #change to rapidjson dir

mkdir build #Put compiler output in build file

cd build #change to build dir

cmake .. #generate Makefile

make #generate other things you can add '-j4' after 'make' to make it faster

sudo make install #generate lib files and put them in /usr/lib(maybe)
```

------



## Compile nanodbc

Before you compile nanodbc, you should download UnixOdbc:

### download mariadb and configure

### download unixodbc

` sudo apt install unixodbc `

### download unixODBC-2.3.0.tar.gz -> to download odbc_config

Download unixODBC-2.3.0.tar.gz ,you can also use `sudo apt install unixodbc ` to download but it's unuseful to my system

```bash
tar -xzvf unixODBC-2.3.0.tar.gz

cd unixODBC-2.3.0.tar.gz

./configure --enable-gui=no

make

sudo make install

odbc_config  --version  #check odbc_config is installed

```

### download odbc-mariadb
{
 `sudo apt install odbc-mariadb`
 
}

### configure odbc-mariadb

see complete info on "https://mariadb.com/kb/en/creating-a-data-source-with-mariadb-connectorodbc/"

[MariaDB ODBC 3.0 Driver]
Description = MariaDB Connector/ODBC v.3.0
Driver=/usr/lib/x86_64-linux-gnu/odbc/libmaodbc.so



```bash
git clone <https://github.com/nanodbc/nanodbc.git> #download nanodbc

cd nanodbc #change to nanodbc dir

mkdir build #Put compiler output in build file

cd build #change to build dir

cmake .. #generate Makefile

make #generate other things you can add '-j4' after 'make' to make it faster

sudo make install #add lib to your env

```
add these statements to your CMakeLists.txt

```cmake
if (NOT TARGET nanodbc)
  find_package(nanodbc CONFIG REQUIRED)
endif()

#add exe here

target_link_libraries(SRC nanodbc)



------



## Compiler muduo

```bash
git clone <https://github.com/chenshuo/muduo.git> #download muduo

cd muduo #change to muduo dir

cp -r muduo ../cids-servser/include #copy include file to projectPath/include

mkdir build #Put compiler output in build file

cd build #change to build dir

cmake .. #generate Makefile

make #generate other things you can add '-j4' after 'make' to make it faster

cd lib #change to lib dir

cp * ../../../cids-servsers/lib #put lib file to projectPath/lib

```

------



## now your project will like this:

- cids-servsers 
  - include 
    - muduo 
      - net 
      - base 
    - nanodbc 
  - lib 
    - libxxx.a      #7/10 lib files


