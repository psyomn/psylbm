#include "db_token.h"
#include "db_user.h"
#include "db_handler.h"

#include <sqlite3.h>

#include <assert.h>
#include <string.h>

#include <unistd.h>

static const char *db_path = __FILE__ ".db";

static int generate_token_test(void)
{
	char *token = psylbm_generate_token();
	const size_t len = strnlen(token, SHA256_DIGEST_LENGTH * 4);

	assert(len == 64);

	free(token);
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
	       | generate_token_test()
	       | free_db(db);
}
