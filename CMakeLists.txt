cmake_minimum_required(VERSION 2.8) 
project(psylbm)

subdirs(src)

set(CMAKE_MODULE_PATH "${psylbm_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_C_FLAGS_DEBUG   "-g ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-O2 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

include_directories("${PROJECT_BINARY_DIR}")

include_directories(${psylbm_SOURCE_DIR}/inc/)
link_directories(${psylbm_SOURCE_DIR}/src/)

find_package(Sqlite3 REQUIRED sqlite3)
find_package(Threads)
find_package(OpenSSL REQUIRED)


