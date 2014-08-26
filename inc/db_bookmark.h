#ifndef DB_BOOKMARK_H
#define DB_BOOKMARK_H

#include <domain.h>
#include <common.h>

int
psy_lbm_insert_bookmark(sqlite3*, uint32_t, char*, 
  char*, uint32_t, uint32_t, uint32_t);

bookmark_t*
psy_lbm_find_bookmark_by_name(sqlite3*, char*);

#endif

