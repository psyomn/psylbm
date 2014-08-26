
# Set version information in a config.h file
set(myproject_VERSION_MAJOR 1)
set(myproject_VERSION_MINOR 0)


set(EXECUTABLE_NAME "psylbm")
add_executable(${EXECUTABLE_NAME} 
main.c
server.c
message_handler.c
db_handler.c
db_bookmark.c
db_token.c
db_user.c
domain.c
)

target_link_libraries(${EXECUTABLE_NAME} 
"${CMAKE_THREAD_LIBS_INIT}"
sqlite3 ssl crypto pthread)
