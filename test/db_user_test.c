#include "db_user.h"
#include "db_handler.h"

#include <sqlite3.h>
#include <unistd.h>

#include <assert.h>

static const char *db_path = "db_user_test.c.db";
static const char *username = "jonny123";
static const char *password = "password123";

static int insert_user_test(sqlite3 *db)
{
	assert(psylbm_insert_user(db, username, password) == 0);
	return 0;
}

static int find_user_by_id_test(sqlite3 *db)
{
	struct user *usr = psylbm_find_user_by_name(db, username);
	uint32_t user_id = usr->id;

	struct user *usr2 = psylbm_find_user(db, user_id);

	assert(usr->id == usr2->id);
	assert(strncmp(usr->name, username, sizeof(*username)) == 0);
	assert(strncmp(usr->name, usr2->name, sizeof(*username)) == 0);

	psylbm_free_user(usr);
	psylbm_free_user(usr2);
	return 0;
}

static int find_user_by_name_test(sqlite3 *db)
{
	struct user *usr = psylbm_find_user_by_name(db, username);

	assert(strncmp(username, usr->name, sizeof(*username)) == 0);
	psylbm_free_user(usr);

	return 0;
}

static int user_exists_test(sqlite3 *db)
{
	const int expect_exists = psylbm_user_exists(db, username);

	assert(expect_exists);

	const int expect_not_exist = !psylbm_user_exists(db, "nope");
	assert(expect_not_exist);

	return 0;
}

static int hash_password_test(void)
{
	char *hpass = psylbm_hash_password("potato", 1);

	assert(strncmp("potato", hpass, SHA256_DIGEST_LENGTH) != 0);
	free(hpass);
	return 0;
}

static int free_db(sqlite3 *db)
{
	return sqlite3_close_v2(db);
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
	       | hash_password_test()

	       | insert_user_test(db)
	       | user_exists_test(db)
	       | find_user_by_name_test(db)
	       | find_user_by_id_test(db)
	       | free_db(db);
}
