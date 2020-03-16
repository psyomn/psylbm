#ifndef DB_USER_H
#define DB_USER_H

#include <common.h>
#include <domain.h>
#include <psylbm.h>

user_t *psy_lbm_find_user(sqlite3 *, uint32_t);

user_t *psy_lbm_find_user_by_name(sqlite3 *, char *);

int   psy_lbm_insert_user(sqlite3 *, char *, char *);

int _psy_lbm_user_exists(sqlite3 *, char *);

char *_psy_lbm_hash_password(char *, int);

#endif // ifndef DB_USER_H
