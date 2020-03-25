#pragma once

#include <common.h>
#include <domain.h>
#include <psylbm.h>

struct user *psylbm_find_user(sqlite3 *, uint32_t);
struct user *psylbm_find_user_by_name(sqlite3 *, char *);
int psylbm_insert_user(sqlite3 *, const char *, const char *);
int psylbm_user_exists(sqlite3 *, char *);
char *psylbm_hash_password(char *, int);
