#pragma once

#include <common.h>

int psylbm_make_token(sqlite3 *, uint32_t, const char *);
int psylbm_set_token(sqlite3 *, const uint32_t, const char *);
char *psylbm_generate_token(void);
uint32_t psylbm_find_user_id_by_token(sqlite3 *, const char *, uint8_t *);
