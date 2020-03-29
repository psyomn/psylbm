#include "db_bookmark.h"
#include "db_handler.h"
#include "protocol.h"
#include "domain.h"

#include <unistd.h>
#include <string.h>
#include <assert.h>

static const char *db_path = "db_bookmark_test.c.db";
static const char *bookmark_name = "don't forget this";
static const char *bookmark_title = "the amazing journeys of jonny";
static const uint32_t bookmark_volume = 1;
static const uint32_t bookmark_chapter = 2;
static const uint32_t bookmark_page = 3;

// factories

void poppulate_mess(struct received_message *mess)
{
	mess->type = MESS_INSERT;

	mess->message.insert.user_id = 123;

	strncpy(&mess->message.insert.name[0],
		bookmark_name,
		sizeof(mess->message.insert.name));

	strncpy(&mess->message.insert.title[0],
		bookmark_title,
		sizeof(mess->message.insert.title));

	mess->message.insert.volume = bookmark_volume;
	mess->message.insert.chapter = bookmark_chapter;
	mess->message.insert.page = bookmark_page;
}

// test code

static int free_db(sqlite3 *db)
{
	return sqlite3_close_v2(db);
}

static int insert_bookmark_test(sqlite3 *db)
{
	struct received_message mess = { 0 };

	poppulate_mess(&mess);

	assert(psylbm_insert_bookmark(db, &mess) == 0);

	return 0;
}

static int find_bookmark_by_name_test(sqlite3 *db)
{
	struct bookmark *bm = NULL;

	bm = psylbm_find_bookmark_by_name(db, bookmark_name);

	assert(bm);

	assert(strncmp(bm->name, bookmark_name, sizeof(*bookmark_name)) == 0);
	assert(strncmp(bm->title, bookmark_title, sizeof(*bookmark_title)) == 0);

	assert(bm->volume == bookmark_volume);
	assert(bm->chapter == bookmark_chapter);
	assert(bm->page == bookmark_page);

	psylbm_free_bookmark(bm);

	return 0;
}

static int find_non_existant_bookmark_by_name_test(sqlite3 *db)
{
	struct bookmark *bm = NULL;

	bm = psylbm_find_bookmark_by_name(db, "nope");
	assert(bm == NULL);
	return 0;
}

static int find_non_existant_bookmark_by_id_test(sqlite3 *db)
{
	struct bookmark *bm = NULL;

	bm = psylbm_find_bookmark(db, 666);
	assert(bm == NULL);
	return 0;
}

static int update_bookmark_test(sqlite3 *db)
{
	struct received_message mess = { 0 };

	poppulate_mess(&mess);

	const char *new_name = "omg wow";
	const char *new_title = "the potato king";

	mess.message.insert.book_id = 1;
	strncpy(&mess.message.insert.name[0], new_name, sizeof(mess.message.insert.name));
	strncpy(&mess.message.insert.title[0], new_title, sizeof(mess.message.insert.title));

	assert(psylbm_update_bookmark(db, &mess) == 0);

	{ // should not exist with old name
		struct bookmark *bm = psylbm_find_bookmark_by_name(db, bookmark_name);
		assert(bm == NULL);
	}

	{ // should exist with new name
		struct bookmark *bm = psylbm_find_bookmark_by_name(db, new_name);
		assert(bm);

		assert(strncmp(bm->name, new_name, sizeof(*new_name)) == 0);
		assert(strncmp(bm->title, new_title, sizeof(*new_title)) == 0);
		psylbm_free_bookmark(bm);
	}

	return 0;
}

static int delete_bookmark_test(sqlite3 *db)
{
	return psylbm_delete_bookmark(db, 1);
}

static int purge_bookmarks_test(sqlite3 *db)
{
	struct received_message mess = { 0 };

	poppulate_mess(&mess);

	psylbm_insert_bookmark(db, &mess);

	return psylbm_purge_bookmarks(db, 1);
}

int main(void)
{
	(void)unlink(db_path);

	psylbm_check_db(db_path);
	sqlite3 *db = NULL;
	if (sqlite3_open(db_path, &db) != SQLITE_OK) {
		perror("problem opening db");
		exit(1);
	}

	return 0
	       | insert_bookmark_test(db)
	       | find_bookmark_by_name_test(db)
	       | find_non_existant_bookmark_by_id_test(db)
	       | find_non_existant_bookmark_by_name_test(db)
	       | update_bookmark_test(db)
	       | delete_bookmark_test(db)
	       | purge_bookmarks_test(db)
	       | free_db(db);
}
