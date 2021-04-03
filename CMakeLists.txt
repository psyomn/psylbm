cmake_minimum_required(VERSION 3.20)
project(psylbm C)

enable_testing()

## Project CC configuration

set(CMAKE_MODULE_PATH "${psylbm_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

set(BUILD_FLAGS ${BUILD_FLAGS} -Wall)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wcast-align)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wwrite-strings)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wunreachable-code)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wno-strict-aliasing)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wno-format-nonliteral)
set(BUILD_FLAGS ${BUILD_FLAGS} -pipe -Werror)
set(BUILD_FLAGS ${BUILD_FLAGS} -march=native)
set(BUILD_FLAGS ${BUILD_FLAGS} -pedantic -Wall)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wno-missing-braces -Wextra)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wno-missing-field-initializers -Wformat=2)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wswitch-default -Wswitch-enum)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wcast-align -Wpointer-arith)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wbad-function-cast -Wstrict-overflow=5)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wstrict-prototypes -Winline)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wundef -Wnested-externs -Wcast-qual)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wshadow -Wunreachable-code)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wlogical-op -Wfloat-equal)
# TODO:
#  will be nice to re-enable this. This will break due to the way
#  sockets are handled right now. Some suggest to use bsd sockets to
#  avoid this problem. Will have to revisit.
# set(BUILD_FLAGS ${BUILD_FLAGS} -Wstrict-aliasing=2)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wredundant-decls)
set(BUILD_FLAGS ${BUILD_FLAGS} -Wold-style-definition -Werror)
set(BUILD_FLAGS ${BUILD_FLAGS} -ggdb3)
set(BUILD_FLAGS ${BUILD_FLAGS} -fno-omit-frame-pointer)
set(BUILD_FLAGS ${BUILD_FLAGS} -ffloat-store -fno-common)
set(BUILD_FLAGS ${BUILD_FLAGS} -fstrict-aliasing)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-g -O2 ${CMAKE_C_FLAGS}")

## Dependencies

find_package(Sqlite3 REQUIRED sqlite3)
find_package(OpenSSL REQUIRED)

## PSYLBM / Sources

include_directories(${psylbm_SOURCE_DIR}/include/)

set(psylbm_SOURCES
  src/server.c
  src/message_handler.c
  src/db_handler.c
  src/db_bookmark.c
  src/db_token.c
  src/db_user.c
  src/domain.c
  src/common.c
  src/protocol.c)

set(PSYLBM_EXECUTABLE_NAME "psylbm")
set(PSYLBM_LINK_LIBS psylbm_shared ${SQLITE3_LIBRARY} ${OPENSSL_LIBRARIES})

add_library(psylbm_shared SHARED ${psylbm_SOURCES})
add_library(psylbm_static STATIC ${psylbm_SOURCES})
target_compile_options(psylbm_shared PRIVATE ${BUILD_FLAGS})
target_compile_options(psylbm_static PRIVATE ${BUILD_FLAGS})

add_executable(${PSYLBM_EXECUTABLE_NAME} src/main.c)
target_compile_options(${PSYLBM_EXECUTABLE_NAME} PRIVATE ${BUILD_FLAGS})
target_link_libraries(${PSYLBM_EXECUTABLE_NAME} ${PSYLBM_LINK_LIBS})

## Formatting / Linting

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

### Testing

function(psylbm_add_test name)
  add_executable(${name} test/${name}.c)
  target_link_libraries(${name} ${PSYLBM_LINK_LIBS})
  target_compile_options(${name} PRIVATE ${BUILD_FLAGS})
  add_test(
    NAME    ${name}_valgrind
    COMMAND valgrind --leak-check=full
                     --error-exitcode=1 $<TARGET_FILE:${name}>)
  add_test(${name} ${name})
endfunction(psylbm_add_test)

psylbm_add_test(protocol_test)
psylbm_add_test(common_test)
psylbm_add_test(domain_test)
psylbm_add_test(db_token_test)
psylbm_add_test(db_user_test)
psylbm_add_test(db_bookmark_test)
