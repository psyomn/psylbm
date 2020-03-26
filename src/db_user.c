#include <openssl/sha.h>
#include <sqlite3.h>

#include <sql_strings.h>
#include <db_user.h>
#include <db_token.h>

struct user *psylbm_find_user(sqlite3 *_db, uint32_t _id)
{
	sqlite3_stmt *stmt = NULL;
	struct user *u = NULL;
	const char **t = NULL;
	int rc;

	u = psylbm_make_user();

	sqlite3_prepare_v2(_db, SQL_FIND_USER_BY_ID,
			   sizeof(SQL_FIND_USER_BY_ID), &stmt, t);

	sqlite3_bind_int(stmt, 1, _id);

	rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW) {
		const unsigned char *bookmark_name_uc = sqlite3_column_text(stmt, 1);
		const int bookmark_name_uc_size = sqlite3_column_bytes(stmt, 1);
		char *converted_bookmark_name = psylbm_strndup(bookmark_name_uc, bookmark_name_uc_size);

		const unsigned char *password_uc = sqlite3_column_text(stmt, 2);
		const int password_uc_size = sqlite3_column_bytes(stmt, 2);
		char *converted_password = psylbm_strndup(password_uc, password_uc_size);

		const unsigned char *salt_uc = sqlite3_column_text(stmt, 3);
		const int salt_uc_size = sqlite3_column_bytes(stmt, 3);
		char *converted_salt = psylbm_strndup(salt_uc, salt_uc_size);

		u->id = sqlite3_column_int(stmt, 0);
		u->name = converted_bookmark_name;
		u->password = converted_password;
		u->salt = converted_salt;
	} else {
		psylbm_free_user(u);
		u = NULL;
	}

	sqlite3_finalize(stmt);
	return u;
}

/* Look for a user. Null if not found */
struct user *psylbm_find_user_by_name(sqlite3 *_db, const char *_name)
{
	sqlite3_stmt *stmt = NULL;
	struct user *u = NULL;
	const char **t = NULL;
	int rc;

	u = psylbm_make_user();

	sqlite3_prepare_v2(_db, SQL_FIND_USER_BY_NAME,
			   sizeof(SQL_FIND_USER_BY_NAME), &stmt, t);

	sqlite3_bind_text(stmt, 1, _name, strlen(_name), SQLITE_STATIC);

	rc = sqlite3_step(stmt);

	/* We only want the first occurence, and duplicate usernames should not exist
	 * due to the unique restriction */
	if (rc == SQLITE_ROW) {
		const unsigned char *bookmark_name_uc = sqlite3_column_text(stmt, 1);
		const int bookmark_name_uc_size = sqlite3_column_bytes(stmt, 1);
		char *converted_bookmark_name = psylbm_strndup(bookmark_name_uc, bookmark_name_uc_size);

		const unsigned char *password_uc = sqlite3_column_text(stmt, 2);
		const int password_uc_size = sqlite3_column_bytes(stmt, 2);
		char *converted_password = psylbm_strndup(password_uc, password_uc_size);

		const unsigned char *salt_uc = sqlite3_column_text(stmt, 3);
		const int salt_uc_size = sqlite3_column_bytes(stmt, 3);
		char *converted_salt = psylbm_strndup(salt_uc, salt_uc_size);

		u->id = sqlite3_column_int(stmt, 0);
		u->name = converted_bookmark_name;
		u->password = converted_password;
		u->salt = converted_salt;
	} else {
		psylbm_free_user(u);
		u = NULL;
	}

	sqlite3_finalize(stmt);

	return u;
}

int psylbm_insert_user(sqlite3 *db, const char *username, const char *password)
{
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;
	char *hashed_password = NULL;
	struct user *u = NULL;
	int salt = time(0);
	char salt_str[11];

	if (psylbm_user_exists(db, username)) return -2;

	sprintf(salt_str, "%d", salt);
	hashed_password = psylbm_hash_password(password, salt);

	sqlite3_prepare_v2(db, SQL_INSERT_USER,
			   sizeof(SQL_INSERT_USER), &stmt, t);

	sqlite3_bind_text(stmt,
			  1,
			  username,
			  strlen(username),
			  SQLITE_STATIC);
	sqlite3_bind_text(stmt,
			  2,
			  hashed_password,
			  strlen(hashed_password),
			  SQLITE_STATIC);
	sqlite3_bind_text(stmt,
			  3,
			  salt_str,
			  strlen(salt_str),
			  SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		perror("Problem inserting user row");
		free(hashed_password);
		sqlite3_finalize(stmt);
		return -1;
	}

	free(hashed_password);
	sqlite3_finalize(stmt);

	/*
	 * Now we insert the token api row - since everyone logged in will have one,
	 * we insert it once to not have to lookup in the future. Therefore to
	 * update/set the api token, we just run an update
	 */

	u = psylbm_find_user_by_name(db, username);
	psylbm_make_token(db, u->id, "new");
	psylbm_free_user(u);

	return 0;
}

int psylbm_user_exists(sqlite3 *db, const char *name)
{
	int ret;
	struct user *usr = NULL;

	usr = psylbm_find_user_by_name(db, name);
	ret = !(usr == NULL);

	psylbm_free_user(usr);

	return ret;
}

/*
 * Thanks to Adam Lamers
 *   http://stackoverflow.com/questions/2262386/
 */
char *psylbm_hash_password(const char *pass, int salt)
{
	unsigned char hashed_pass[SHA256_DIGEST_LENGTH];
	char *ret_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);

	char string[50 + 10 + 1] = { 0 };

	/* 10 is since epoch bytes for salt */
	char since_epoch[11] = { 0 };
	int i = 0;

	/* Build salted string */
	memset(string, 0, sizeof(string));
	sprintf(since_epoch, "%d", salt);
	strcpy(string, pass);
	strcat(string, since_epoch);

	/* And get the hashed value */
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, string, strlen(string));
	SHA256_Final(hashed_pass, &sha256);

	for (i = 0; i < SHA256_DIGEST_LENGTH; ++i)
		sprintf((ret_string + (i * 2)), "%02x", hashed_pass[i]);

	return ret_string;
}
