# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
else ()
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lsqlite3 -lpthread -g -Wall")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
else ()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()

# Set version information in a config.h file
set(myproject_VERSION_MAJOR 1)
set(myproject_VERSION_MINOR 0)

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_MODULE_PATH "${psylbm_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})
find_package(Sqlite3 REQUIRED sqlite3)
find_package(Threads)

# Define sources and executable
include_directories(${psylbm_SOURCE_DIR}/inc/)
link_directories(${psylbm_SOURCE_DIR}/src/)
set(EXECUTABLE_NAME "psylbm")
add_executable(${EXECUTABLE_NAME} 
main.c
server.c
message_handler.c
db_handler.c
)

target_link_libraries(${EXECUTABLE_NAME} "${CMAKE_THREAD_LIBS_INIT}")
