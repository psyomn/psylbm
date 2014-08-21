#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#define PSYLBM_STARTED   1
#define PSYLBM_LISTENING 2
#define PSYLBM_PAUSED    3 
#define PSYLBM_STOPPED   4
#define PSYLBM_PANIC    -1

struct psy_lbm_status_strings {
  int   id;
  char* status;
};

typedef struct psy_lbm_status_strings psy_lbm_status_strings_t;

struct psy_lbm_server {
  uint16_t portnum;
  char*    hostname;
  int      status;
};

typedef struct psy_lbm_server psy_lbm_server_t;

psy_lbm_server_t*
psy_lbm_make_server(uint16_t, char*);

void
psy_lbm_server_listen(psy_lbm_server_t*);

void
psy_lbm_print_server_info(psy_lbm_server_t*);

char* 
psy_lbm_server_status(psy_lbm_server_t*);


#endif

