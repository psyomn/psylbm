#include "sql_strings.h"
#include "db_token.h"
#include "domain.h"

/** This should only be called once, from user creation */
int psylbm_make_token(sqlite3 *db, uint32_t user_id, const char *token)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(db, SQL_INSERT_API, sizeof(SQL_INSERT_API), &stmt, t);
	sqlite3_bind_int(stmt, 1, user_id);
	sqlite3_bind_text(stmt, 2, token, strlen(token), SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		perror("problem inserting api token");
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

/** Update login token */
int psylbm_set_token(sqlite3 *db, uint32_t user_id, char *token)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(db, SQL_UPDATE_TOKEN, sizeof(SQL_UPDATE_TOKEN), &stmt, t);
	sqlite3_bind_text(stmt, 1, token, strlen(token), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, user_id);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		perror("Problem setting api token");
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

char *psylbm_generate_token(void)
{
	char rand_data[128] = { 0 };
	unsigned char hashed[SHA256_DIGEST_LENGTH] = { 0 };

	char *ret_string = calloc(SHA256_DIGEST_LENGTH * 2 + 1, sizeof(char));

	if (!ret_string) return NULL;

	for (size_t i = 0; i < sizeof(rand_data); ++i)
		rand_data[i] = rand() & 0xff;

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, rand_data, 128);
	SHA256_Final(hashed, &sha256);

	for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		sprintf((ret_string + (i * 2)), "%02x", hashed[i]);

	return ret_string;
}

uint32_t psylbm_find_user_id_by_token(sqlite3 *db, const char *token, uint8_t *error)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;
	int ret;

	sqlite3_prepare_v2(
		db, SQL_FIND_USER_BY_TOKEN, sizeof(SQL_FIND_USER_BY_TOKEN),
		&stmt, t);

	sqlite3_bind_text(stmt, 1, token, strlen(token), SQLITE_STATIC);

	ret = sqlite3_step(stmt);

	if (ret == SQLITE_ROW) {
		int32_t id = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		return id;
	}

	sqlite3_finalize(stmt);
	perror("Problem finding user by id");
	*error = 1;

	return 0;
}
