#ifndef DB_BOOKMARK_H
#define DB_BOOKMARK_H

#include <common.h>

int
psy_lbm_insert_bookmark(sqlite3*, uint32_t, char*, 
  char*, uint32_t, uint32_t, uint32_t);

#endif

