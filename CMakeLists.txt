cmake_minimum_required(VERSION 3.00)
project(cids_server_new)

#Global Setting
SET(CMAKE_CXX_FLAGS -pthread)

SET(CMAKE_CXX_STANDARD  14)

#Thir Lib

    #find- nanodbc
if(NOT TARGET nanodbc)
    find_package(nanodbc CONFIG REQUIRED)
endif()

    #find- rapidjson
find_package(RapidJSON REQUIRED)

    #already put muduo in the project

#add include
include_directories(Include)
include_directories(Include/Third_Lib)

include_directories(${RapidJSON_INCLUDE_DIR})
#muduo in Include/Third_lib
#nanodbc in Include/Third_lib

#add lib
link_directories(Lib)

#My src
aux_source_directory(Src/Center CENTER_SRC)
aux_source_directory(Src/Mirror MIRROR_SRC)
aux_source_directory(Src/Common COMMON_SRC)

#生成可执行文件

add_executable(center ${CENTER_SRC} ${COMMON_SRC})
add_executable(mirror ${MIRROR_SRC} ${COMMON_SRC})

#Center绑定包
target_link_libraries(center libmuduo_base.a libmuduo_net.a) #绑定muduo包


#Center修改输出属性
set_target_properties(center PROPERTIES OUTPUT_NAME cids_Center)
set_target_properties(center PROPERTIES PREFIX "")
set_target_properties(center PROPERTIES SUFFIX "")


#Mirror绑定包
target_link_libraries(mirror libmuduo_base.a libmuduo_net.a) #绑定muduo包
target_link_libraries(mirror ${RapidJSON_LIBS}) #绑定rapidjson包
target_link_libraries(mirror nanodbc) #绑定nanodbc包

#Mirror修改输出树形
set_target_properties(mirror PROPERTIES OUTPUT_NAME cids_Mirror)
set_target_properties(mirror PROPERTIES PREFIX "")
set_target_properties(mirror PROPERTIES SUFFIX "")
