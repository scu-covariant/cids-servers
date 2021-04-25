# cids-servers
center server ,mirror server and driver


dir
----cids-server
----rapidjson
----nanodbc
----muduo

after clone this project

Imporve projectPath

-- mkdir lib   #to find libraries
-- mkdir include   #to find headers

Compile rapidJson

git clone https://github.com/Tencent/rapidjson.git  #download rapidjson

cd rapidjson                                        #change to rapidjson dir

mkdir build                                         #Put compiler output in build file

cd build                                            #change to build dir

cmake ..                                            #generate Makefile

make                                                #generate other things you can add '-j4' after 'make' to make it faster

sudo make install                                   #generate lib files and put them in /usr/lib(maybe)


Compile nanodbc

git clone https://github.com/nanodbc/nanodbc.git    #download nanodbc

cd nanodbc                                          #change to nanodbc dir

cp -r nanodbc ../cids-servser/include               #copy include file to projectPath/include 

mkdir build                                         #Put compiler output in build file

cd build                                            #change to build dir

cmake ..                                            #generate Makefile

make                                                #generate other things you can add '-j4' after 'make' to make it faster

cp libnanodbc.a ../../cids-servers/lib              #copy library file to projectPath/lib


Compiler muduo

git clone https://github.com/chenshuo/muduo.git     #download muduo

cd muduo                                            #change to muduo dir

cp -r muduo ../cids-servser/include                 #copy include file to projectPath/include

mkdir build                                         #Put compiler output in build file

cd build                                            #change to build dir

cmake ..                                            #generate Makefile

make                                                #generate other things you can add '-j4' after 'make' to make it faster

cd lib                                              #change to lib dir

cp * ../../../cids-servsers/lib                     #put lib file to projectPath/lib


now your project will like this:

cids-servsers
----include
    ----muduo
        ----net
        ----base
    ----nanodbc
----lib
    ----libxxx.a                                    #10 lib files




