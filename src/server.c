#include "server.h"
#include "message_handler.h"
#include "db_handler.h"
#include "common.h"

#include <sqlite3.h>

static psy_lbm_status_strings_t psy_lbm_statuses[] = {
  {PSYLBM_STARTED,   "started"},
  {PSYLBM_LISTENING, "listening"}, 
  {PSYLBM_PAUSED,    "paused"},
  {PSYLBM_SHUTDOWN,  "shutting down"},
  {PSYLBM_PANIC,     "panic"}
};


psy_lbm_server_t* 
psy_lbm_make_server(uint16_t _portnum, char* _host) {
  psy_lbm_server_t* server = malloc(sizeof(psy_lbm_server_t));
  server->portnum = _portnum;
  server->hostname = _host;
  server->status = PSYLBM_STARTED;
  return server;
}

void
psy_lbm_server_listen(psy_lbm_server_t* _server) {
  int fd;

  printf("Trying to listen ... ");

  /* Open socket and do black magic */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    _server->status = PSYLBM_PANIC;
    perror("Problem opening socket");
    return;
  }

  printf("[ok]\n");

  const int buffsize = 2048;
  char buffer[buffsize];
  int recvlen;

  /* Sock info */
  struct sockaddr_in remote_address;
  socklen_t address_length = sizeof(remote_address);
  struct sockaddr_in server_sock;
  server_sock.sin_family = AF_INET;
  server_sock.sin_port = htons(_server->portnum);
  server_sock.sin_addr.s_addr = htonl(INADDR_ANY);

  /* bind */
  if (bind(fd, (struct sockaddr*)&server_sock, sizeof(server_sock)) < 0) {
    perror("Bind failed");
    return;
  }

  if (sqlite3_open(PSYLBM_DB_NAME, &_server->db) != SQLITE_OK) {
    perror("Problem opening db");
    exit(0);
  }

  while (_server->status != PSYLBM_SHUTDOWN) {
    recvlen = recvfrom(
      fd, buffer, buffsize, 0,
      (struct sockaddr*)&remote_address, &address_length);

    if (recvlen == -1) {
      perror("Problem receiving");
      continue;
    }

    printf("Received %d bytes\n", recvlen);
    printf("[%s]\n", buffer);
    psy_lbm_handle_message(buffer);
    memset((void*) buffer, 0, sizeof(buffer));
  }

  _server->status = PSYLBM_LISTENING;
  printf("[ok]\n");
}

void
psy_lbm_print_server_info(psy_lbm_server_t* _server) {
  printf("%s: %d\n" "%s: %s\n" "%s: %s\n",
    "port", _server->portnum,
    "host", _server->hostname,
    "status", psy_lbm_server_status(_server));
}

char* 
psy_lbm_server_status(psy_lbm_server_t* _server) {
  size_t size = sizeof(psy_lbm_statuses) / sizeof(psy_lbm_status_strings_t);
  size_t index;
  char* ret;

  for (index = 0; index < size; --index) {
    if(psy_lbm_statuses[index].id == _server->status) return psy_lbm_statuses[index].status;
  }

  return "status-find-error";
}
