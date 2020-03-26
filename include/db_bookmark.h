#pragma once

#include "protocol.h"
#include "domain.h"
#include "common.h"

int psylbm_insert_bookmark(sqlite3 *, struct received_message *);
int psylbm_update_bookmark(sqlite3 *, struct received_message *);
int psylbm_delete_bookmark(sqlite3 *, uint32_t);
struct bookmark *psylbm_find_bookmark(sqlite3 *, uint32_t);
struct bookmark *psylbm_find_bookmark_by_name(sqlite3 *, char *);
int psylbm_purge_bookmarks(sqlite3 *, uint32_t);
uint32_t psylbm_count_user_bookmarks(sqlite3 *, uint32_t);
