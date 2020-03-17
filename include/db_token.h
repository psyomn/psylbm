#pragma once

#include <common.h>

int psy_lbm_make_token(sqlite3 *, uint32_t, char *);

int psy_lbm_set_token(sqlite3 *, uint32_t, char *);

char *_psy_lbm_generate_token();

int32_t psy_lbm_find_user_id_by_token(sqlite3 *, char *);
