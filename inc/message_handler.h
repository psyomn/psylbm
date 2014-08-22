#ifndef PSYLBM_MESSAGE_HANDLER_H
#define PSYLBM_MESSAGE_HANDLER_H

#include <stdint.h>

void
psy_lbm_handle_message(char*);

char*
psy_lbm_handle_authorization(char*, char*);

int
psy_lbm_handle_insert(char*,uint32_t,uint32_t,uint32_t,char*);

int
psy_lbm_handle_register(char*,char*);

int
psy_lbm_handle_error(char*);

#endif

