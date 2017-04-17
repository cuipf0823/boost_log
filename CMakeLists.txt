cmake_minimum_required(VERSION 3.6)
project(boost_demo)

set(CMAKE_CXX_STANDARD 11)
set(src_path, ${CMAKE_SOURCE_DIR}/)
include_directories(${src_path})

set(SOURCE_FILES
        ${src_path}src/boost_log.cc
        ${src_path}src/complex.h
        ${src_path}src/simple.h
        )

add_definitions("-ggdb -fPIC -ffunction-sections -O0 -std=c++11 -DBOOST_LOG_DYN_LINK")
add_executable(boost_demo ${SOURCE_FILES})
target_link_libraries(boost_demo "boost_log_setup" "boost_log" "boost_thread" "boost_system")