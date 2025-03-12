cmake_minimum_required(VERSION 3.19)
project(MyProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER "D:/Program Files/mingw64_w64devkit-1.23.0/w64devkit/bin/gcc.exe")
set (CMAKE_CXX_COMPILER "D:/Program Files/mingw64_w64devkit-1.23.0/w64devkit/bin/g++.exe")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# include_directories(./csos-teach/src/inc/csos)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE TMPSOURCES "./src/kernel32/*.c")
list(APPEND SOURCES ${TMPSOURCES})
# APPEND(SOURCES,TMPSOURCES)
# file(GLOB_RECURSE TMPSOURCES "./src/kernel32/*.S")
# list(APPEND SOURCES ${TMPSOURCES})

file(GLOB_RECURSE TMPSOURCES "./src/lib/*.c")
list(APPEND SOURCES ${TMPSOURCES})

include_directories(./src/inc)
include_directories(./src/inc/csos)
MESSAGE("SOURCES:${SOURCES}xx")
# 添加可执行文件
add_executable(MyProject 
        ${SOURCES})