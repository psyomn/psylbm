#ifndef DB_HANDLER_H
#define DB_HANDLER_h

#include <sqlite3.h>
#include "domain.h"

#define PSYLBM_DB_NAME "psy-lightbookmark.sqlite3"

void
psy_lbm_check_db();

void
psy_lbm_make_db();

user_t*
psy_lbm_find_user(sqlite3*, uint32_t);

user_t*
psy_lbm_find_user_by_name(sqlite3*, char*);

int
psy_lbm_insert_user(sqlite3*, char*, char*);

void
psy_lbm_insert_bookmark(sqlite3*, uint32_t, char*, 
  uint32_t, uint32_t, uint32_t);

int
psy_lbm_make_token(sqlite3*, uint32_t, char*);

int
psy_lbm_set_token(sqlite3*, uint32_t, char*);

int
_psy_lbm_user_exists(sqlite3*, char*);

char*
_psy_lbm_hash_password(char*, int);

char*
_psy_lbm_generate_token();

#endif

