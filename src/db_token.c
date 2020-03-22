#include <sql_strings.h>
#include <db_token.h>
#include <domain.h>

/** This should only be called once, from user creation */
int psy_lbm_make_token(sqlite3 *_db, uint32_t _user_id, const char *_token)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(_db, SQL_INSERT_API, sizeof(SQL_INSERT_API), &stmt, t);
	sqlite3_bind_int(stmt, 1, _user_id);
	sqlite3_bind_text(stmt, 2, _token, strlen(_token), SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		perror("Problem inserting api token");
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

/** Update login token */
int psy_lbm_set_token(sqlite3 *_db, uint32_t _user_id, char *_token)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;

	sqlite3_prepare_v2(_db, SQL_UPDATE_TOKEN, sizeof(SQL_UPDATE_TOKEN), &stmt, t);
	sqlite3_bind_text(stmt, 1, _token, strlen(_token), SQLITE_STATIC);
	sqlite3_bind_int(stmt, 2, _user_id);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		perror("Problem setting api token");
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

char *_psy_lbm_generate_token(void)
{
	int buffsize = 128;
	char rand_data[buffsize];
	int max = buffsize;
	int i;
	unsigned char hashed[SHA256_DIGEST_LENGTH];
	char *ret_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);

	/* Fill in a random string */
	for (i = 0; i < max; ++i)
		rand_data[i] = rand() % 0xFF;

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, rand_data, strlen(rand_data));
	SHA256_Final(hashed, &sha256);

	for (i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		sprintf((ret_string + (i * 2)), "%02x", hashed[i]);

	return ret_string;
}

uint32_t psy_lbm_find_user_id_by_token(sqlite3 *_db, char *_token, uint8_t *error)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;
	int ret;

	sqlite3_prepare_v2(
		_db, SQL_FIND_USER_BY_TOKEN, sizeof(SQL_FIND_USER_BY_TOKEN),
		&stmt, t);

	sqlite3_bind_text(stmt, 1, _token, strlen(_token), SQLITE_STATIC);

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
