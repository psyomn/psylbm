cmake_minimum_required(VERSION 2.8)
project(psylbm)

enable_testing()

## Project CC configuration

set(CMAKE_MODULE_PATH "${psylbm_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunreachable-code")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-nonliteral")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -g -O3 -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers -Wformat=2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default -Wswitch-enum")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Wpointer-arith")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wbad-function-cast -Wstrict-overflow=5")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes -Winline")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wundef -Wnested-externs -Wcast-qual")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow -Wunreachable-code")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op -Wfloat-equal")
# TODO:
#  will be nice to re-enable this. This will break due to the way
#  sockets are handled right now. Some suggest to use bsd sockets to
#  avoid this problem. Will have to revisit.
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-aliasing=2"
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wredundant-decls")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wold-style-definition -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffloat-store -fno-common")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing")
set(CMAKE_C_FLAGS_DEBUG "-g ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS}")

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

set(PSYLBM_EXECUTABLE_NAME "psylbm-server")
set(PSYLBM_LINK_LIBS psylbm ${SQLITE3_LIBRARY} ${OPENSSL_LIBRARIES})

add_library(psylbm SHARED ${psylbm_SOURCES})
add_executable(${PSYLBM_EXECUTABLE_NAME} src/main.c)
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
