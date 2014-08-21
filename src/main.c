#include "common.h"
#include "server.h"

static pthread_t __main_thread;
static void* __main_thread_status;

void* 
start(void* data) {
  psy_lbm_server_t* server = NULL;
  server = psy_lbm_make_server(8080, "hello");

  psy_lbm_print_server_info(server);
  psy_lbm_server_listen(server);

  pthread_exit(NULL);
}

int 
main(int argc, char* argv[]) {
  pthread_create(&__main_thread, NULL, start, NULL);

  pthread_join(__main_thread, __main_thread_status);
  return 0;
}

