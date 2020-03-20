cmake_minimum_required(VERSION 2.8)
project(psylbm)

enable_testing()

subdirs(src)

set(CMAKE_MODULE_PATH "${psylbm_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_C_FLAGS_DEBUG   "-g ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-O2 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

include_directories("${PROJECT_BINARY_DIR}")

include_directories(${psylbm_SOURCE_DIR}/include/)
# link_directories(${psylbm_SOURCE_DIR}/src/)

find_package(Sqlite3 REQUIRED sqlite3)
find_package(OpenSSL REQUIRED)

add_custom_target(
  format
    COMMAND
    uncrustify -c ${CMAKE_SOURCE_DIR}/uncrustify.cfg --no-backup ${CMAKE_SOURCE_DIR}/**/*.c ${CMAKE_SOURCE_DIR}/include/*.h
  )

add_custom_target(
  check-format
    COMMAND
    uncrustify -c ${CMAKE_SOURCE_DIR}/uncrustify.cfg --check ${CMAKE_SOURCE_DIR}/**/*.c ${CMAKE_SOURCE_DIR}/include/*.h
  )


function(psylbm_add_test name)
  add_executable(${name} test/${name}.c)
  target_link_libraries(${name} psylbm sqlite3 ssl crypto)
  add_test(
    NAME    ${name}_valgrind
    COMMAND valgrind --leak-check=full
                     --error-exitcode=1 $<TARGET_FILE:${name}>)
  add_test(${name} ${name})
endfunction(psylbm_add_test)

psylbm_add_test(protocol_test)
