#1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.1)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(rpc_test)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES(
.
)

#设置编译器版本
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

#5.设置环境变量，编译用到的源文件全部都要放到这里
SET(TEST_MATH
${DIR_SRCS}
)

#6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_MATH})

#7.添加库的名称
TARGET_LINK_LIBRARIES(${PROJECT_NAME} curl glog)
