#ifndef DB_BOOKMARK_H
#define DB_BOOKMARK_H

#include <domain.h>
#include <common.h>

int psy_lbm_insert_bookmark(
	sqlite3 *,
	uint32_t,
	char *,
	char *,
	uint32_t,
	uint32_t,
	uint32_t);

int psy_lbm_update_bookmark(
	sqlite3 *,
	char *,
	char *,
	uint32_t,
	uint32_t,
	uint32_t,
	uint32_t);

bookmark_t *psy_lbm_find_bookmark_by_name(sqlite3 *, char *);

bookmark_t *psy_lbm_find_bookmark(sqlite3 *, uint32_t);

int psy_lbm_delete_bookmark(sqlite3 *, uint32_t);

/* Given a user id, delete all the rows with that user id of bookmarks */
int psy_lbm_purge_bookmarks(sqlite3 *, uint32_t);

uint32_t psy_lbm_count_user_bookmarks(sqlite3 *, uint32_t);

#endif // ifndef DB_BOOKMARK_H
