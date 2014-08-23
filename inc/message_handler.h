#ifndef PSYLBM_MESSAGE_HANDLER_H
#define PSYLBM_MESSAGE_HANDLER_H

#include <stdint.h>

#include <server.h>

void
psy_lbm_handle_message(psy_lbm_server_t*, char*);

char*
psy_lbm_handle_authorization(psy_lbm_server_t*, char*, char*);

int
psy_lbm_handle_insert(psy_lbm_server_t*, char*, uint32_t,
  uint32_t, uint32_t, char*);

int
psy_lbm_handle_register(psy_lbm_server_t*, char*, char*);

int
psy_lbm_handle_error(char*);

#endif

