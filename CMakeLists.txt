cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
project(clog)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(util/include)
#set(INSTALL_PREFIX /home/cyz/CLionProjects/install)
#set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})

add_subdirectory(util/src)

add_subdirectory(daemon)

