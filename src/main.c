#include "common.h"
#include "server.h"
#include "db_handler.h"

void* start(void* data);
void init();

int 
main(int argc, char* argv[]) {
  static pthread_t __main_thread;
  static void* __main_thread_status;

  /* Init psylbm */
  init();

  /* Start the neverending listen of server */
  pthread_create(&__main_thread, NULL, start, NULL);
  pthread_join(__main_thread, __main_thread_status);
  return 0;
}

void* 
start(void* data) {
  psy_lbm_server_t* server = NULL;
  server = psy_lbm_make_server(8080, "localhost");

  psy_lbm_print_server_info(server);
  psy_lbm_server_listen(server);

  pthread_exit(NULL);
}

/* Any needed init goes here */
void
init() {
  psy_lbm_check_db();
}

