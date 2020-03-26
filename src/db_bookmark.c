#include "protocol.h"

#include "db_bookmark.h"
#include "sql_strings.h"

int psylbm_insert_bookmark(sqlite3 *db, struct received_message *recv_mess)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(db, SQL_INSERT_BOOKMARK,
			   sizeof(SQL_INSERT_BOOKMARK), &stmt, t);

	sqlite3_bind_int(stmt, 1, recv_mess->message.insert.user_id);

	sqlite3_bind_text(stmt, 2, recv_mess->message.insert.name,
			  strlen(recv_mess->message.insert.name),
			  SQLITE_STATIC);

	sqlite3_bind_text(stmt, 3, recv_mess->message.insert.title,
			  strlen(recv_mess->message.insert.title),
			  SQLITE_STATIC);

	sqlite3_bind_int(stmt, 4, recv_mess->message.insert.volume);
	sqlite3_bind_int(stmt, 5, recv_mess->message.insert.chapter);
	sqlite3_bind_int(stmt, 6, recv_mess->message.insert.page);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		printf("problem inserting bookmark [%d] [%s] [%s]\n",
		       recv_mess->message.insert.user_id,
		       recv_mess->message.insert.name,
		       recv_mess->message.insert.title);
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

int psylbm_update_bookmark(sqlite3 *db, struct received_message *mess)
{
	int ret = 0;
	struct bookmark *bm = NULL;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	if (mess->message.insert.book_id == 0)
		bm = psylbm_find_bookmark_by_name(db, &mess->message.insert.name[0]);
	else
		bm = psylbm_find_bookmark(db, mess->message.insert.book_id);

	sqlite3_prepare_v2(db, SQL_UPDATE_BOOKMARK_VALUES,
			   sizeof(SQL_UPDATE_BOOKMARK_VALUES), &stmt, t);

	sqlite3_bind_text(stmt, 1,
			  mess->message.insert.name,
			  strlen(mess->message.insert.name),
			  SQLITE_STATIC);

	sqlite3_bind_text(stmt, 2,
			  mess->message.insert.title,
			  strlen(mess->message.insert.title),
			  SQLITE_STATIC);

	sqlite3_bind_int(stmt, 3, mess->message.insert.volume);
	sqlite3_bind_int(stmt, 4, mess->message.insert.chapter);
	sqlite3_bind_int(stmt, 5, mess->message.insert.page);
	sqlite3_bind_int(stmt, 6, bm->id);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		printf("error updating bookmark\n");
		ret = -1;
	}

	psylbm_free_bookmark(bm);
	sqlite3_finalize(stmt);
	return ret;
}

struct bookmark *psylbm_find_bookmark_by_name(sqlite3 *_db, char *_name)
{
	struct bookmark *book = NULL;
	const char **t = NULL;
	sqlite3_stmt *stmt = NULL;

	sqlite3_prepare_v2(_db, SQL_FIND_BOOKMARK_BY_NAME,
			   sizeof(SQL_FIND_BOOKMARK_BY_NAME), &stmt, t);

	sqlite3_bind_text(stmt, 1, _name, strlen(_name), SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_ROW)
		return NULL;

	// TODO: Fix utf strings
	const unsigned char *bookmark_name_uc = sqlite3_column_text(stmt, 2);
	const int bookmark_name_uc_size = sqlite3_column_bytes(stmt, 2);
	char *converted_bookmark_name = psylbm_strndup(bookmark_name_uc, bookmark_name_uc_size);

	const unsigned char *title_uc = sqlite3_column_text(stmt, 3);
	const int title_uc_size = sqlite3_column_bytes(stmt, 3);
	char *converted_title = psylbm_strndup(title_uc, title_uc_size);

	book = psylbm_make_bookmark();
	book->id = sqlite3_column_int(stmt, 0);
	book->user_id = sqlite3_column_int(stmt, 1);
	book->name = converted_bookmark_name;
	book->title = converted_title;
	book->volume = sqlite3_column_int(stmt, 4);
	book->chapter = sqlite3_column_int(stmt, 5);
	book->page = sqlite3_column_int(stmt, 6);

	sqlite3_finalize(stmt);

	return book;
}

int psylbm_delete_bookmark(sqlite3 *_db, uint32_t _book_id)
{
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	printf("Delete bookmark with id: [%d]\n", _book_id);
	sqlite3_prepare_v2(_db, SQL_DELETE_BOOKMARK, sizeof(SQL_DELETE_BOOKMARK),
			   &stmt, t);

	sqlite3_bind_int(stmt, 1, _book_id);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		ret = -1;

	sqlite3_finalize(stmt);
	return ret;
}

int psylbm_purge_bookmarks(sqlite3 *_db, uint32_t _user_id)
{
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(_db, SQL_PURGE_BOOKMARKS, sizeof(SQL_PURGE_BOOKMARKS),
			   &stmt, t);

	sqlite3_bind_int(stmt, 1, _user_id);

	if (sqlite3_step(stmt) != SQLITE_DONE)
		ret = -1;

	sqlite3_finalize(stmt);
	return ret;
}

struct bookmark *psylbm_find_bookmark(sqlite3 *_db, uint32_t _bm_id)
{
	struct bookmark *book = NULL;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(_db, SQL_FIND_BOOKMARK,
			   sizeof(SQL_FIND_BOOKMARK), &stmt, t);

	sqlite3_bind_int(stmt, 1, _bm_id);

	if (sqlite3_step(stmt) != SQLITE_ROW) {
		printf("Did not find bookmark with id [%d]\n", _bm_id);
		return NULL;
	}

	// TODO: Fix utf strings
	const unsigned char *bookmark_name_uc = sqlite3_column_text(stmt, 2);
	const int bookmark_name_uc_size = sqlite3_column_bytes(stmt, 2);
	char *converted_bookmark_name = psylbm_strndup(bookmark_name_uc, bookmark_name_uc_size);

	const unsigned char *title_uc = sqlite3_column_text(stmt, 3);
	const int title_uc_size = sqlite3_column_bytes(stmt, 3);
	char *converted_title = psylbm_strndup(title_uc, title_uc_size);

	book = psylbm_make_bookmark();
	book->id = sqlite3_column_int(stmt, 0);
	book->user_id = sqlite3_column_int(stmt, 1);
	book->name = converted_bookmark_name;
	book->title = converted_title;
	book->volume = sqlite3_column_int(stmt, 4);
	book->chapter = sqlite3_column_int(stmt, 5);
	book->page = sqlite3_column_int(stmt, 6);

	sqlite3_finalize(stmt);

	return book;
}

uint32_t psylbm_count_user_bookmarks(sqlite3 *db, uint32_t user_id)
{
	int stat;
	uint32_t count = 0;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(db, SQL_COUNT_USER_BOOKMARKS,
			   sizeof(SQL_COUNT_USER_BOOKMARKS), &stmt, t);

	sqlite3_bind_int(stmt, 1, user_id);
	stat = sqlite3_step(stmt);

	if (stat == SQLITE_ROW)
		count = sqlite3_column_int(stmt, 0);

	else
		printf("Problem counting bookmarks\n");

	sqlite3_finalize(stmt);

	return count;
}
