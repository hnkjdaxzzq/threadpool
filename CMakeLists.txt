cmake_minimum_required(VERSION 3.10)  # 指定 CMake 最低版本
project(MyProject)                      # 项目名称

set(CMAKE_CXX_STANDARD 17)              # 设置 C++ 标准

add_executable(MyExecutable main.cpp)  # 创建可执行文件
