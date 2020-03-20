set(myproject_VERSION_MAJOR 1)
set(myproject_VERSION_MINOR 0)

set(psylbm_SOURCES
  server.c
  message_handler.c
  db_handler.c
  db_bookmark.c
  db_token.c
  db_user.c
  domain.c
  protocol.c)

add_library(psylbm STATIC ${psylbm_SOURCES})

set(EXECUTABLE_NAME "psylbm-server")

add_executable(${EXECUTABLE_NAME} main.c)
target_link_libraries(${EXECUTABLE_NAME} "${CMAKE_THREAD_LIBS_INIT}" sqlite3 ssl crypto psylbm)
