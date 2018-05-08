cmake_minimum_required(VERSION 3.10)
project(clog)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(${PROJECT_SOURCE_DIR})

#add_executable(clog daemon/main.cpp daemon/Server.h util/log.h util/tools.h daemon/main_test.cpp)

add_executable(clog_test daemon/main_test.cpp daemon/Server.cpp daemon/Server.h util/include/log.h util/include/tools.h util/src/tools.cpp util/src/log.cpp daemon/Servant.cpp daemon/Servant.h util/src/Vin_Thread.cpp util/include/Vin_Thread.h daemon/testThread.cpp daemon/testThread.h util/src/Vin_Exception.cpp util/include/Vin_Exception.h)

TARGET_LINK_LIBRARIES(clog_test pthread)