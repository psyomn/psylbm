#include <db_handler.h>
#include <common.h>
#include <sql_strings.h>
#include <psylbm.h>

#include <openssl/sha.h>
#include <sqlite3.h>

/* Check if db exists - create if not */
void
psy_lbm_check_db() {
  FILE* fd; 

  fd = fopen(PSYLBM_DB_NAME, "r");

  if (!fd) {
    /* db dne, so make db */
    psy_lbm_make_db();
  }
}

void
psy_lbm_make_db() {
  sqlite3* db; 
  int rc; 
  char* error = 0;
  char * sql_strs[] = {
    SQL_CREATE_USERS, 
    SQL_CREATE_BOOKMARKS, 
    SQL_CREATE_API_TOKENS};
  int sql_ix;
  int sql_max = sizeof(sql_strs) / sizeof(char*);

  printf("Making database for the first time ... \n");

  sqlite3_open(PSYLBM_DB_NAME, &db);

  for (sql_ix = 0; sql_ix < sql_max; ++sql_ix) {
    rc = sqlite3_exec(db, sql_strs[sql_ix], 
                      NULL, 0, &error);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL ERROR: %s\n", error);
      sqlite3_free(error);
    }

  }
  sqlite3_close(db);
}

user_t* 
psy_lbm_find_user(sqlite3* _db, uint32_t _id) {
  sqlite3_stmt* stmt = NULL;
  user_t* u = NULL;
  const char** t = NULL;
  int rc;

  u = psy_lbm_make_user();

  sqlite3_prepare_v2(_db, SQL_FIND_USER_BY_ID,
                     sizeof(SQL_FIND_USER_BY_ID), &stmt, t);

  sqlite3_bind_int(stmt, 1, _id);

  rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW) {
    u->id = sqlite3_column_int(stmt, 0);
    u->name = strdup((char*) sqlite3_column_text(stmt, 1));
    u->password = strdup((char*) sqlite3_column_text(stmt, 2));
    u->salt = strdup((char*) sqlite3_column_text(stmt, 3));
  }
  else {
    psy_lbm_free_user(u);
    u = NULL;
  }

  sqlite3_finalize(stmt);
  return u;
};

/* Look for a user. Null if not found */
user_t*
psy_lbm_find_user_by_name(sqlite3* _db, char* _name) {
  sqlite3_stmt* stmt = NULL;
  user_t* u = NULL;
  const char** t = NULL;
  int rc;

  u = psy_lbm_make_user();
  u->name = strdup(_name);

  sqlite3_prepare_v2(_db, SQL_FIND_USER_BY_NAME, 
                     sizeof(SQL_FIND_USER_BY_NAME), &stmt, t);

  sqlite3_bind_text(stmt, 1, _name, strlen(u->name), SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);

  /* We only want the first occurence, and duplicate usernames should not exist 
   * due to the unique restriction */
  if (rc == SQLITE_ROW) {
    u->id       = sqlite3_column_int(stmt, 0);
    u->name     = strdup((char*)sqlite3_column_text(stmt, 1));
    u->password = strdup((char*)sqlite3_column_text(stmt, 2));
    u->salt     = strdup((char*)sqlite3_column_text(stmt, 3));
  }
  else {
    psy_lbm_free_user(u);
    u = NULL;
  }

  sqlite3_finalize(stmt);

  return u;
}

int
psy_lbm_insert_user(sqlite3* _db, char* _username, char* _password) {
  sqlite3_stmt*  stmt = NULL;
  const char**   t = NULL;
  char* hashed_password = NULL;
  user_t*        u = NULL;
  int            salt = time(0);
  char           salt_str[11];

  /* Before anything, make sure that the user doesn't exist */
  if (_psy_lbm_user_exists(_db, _username))
    return -2;
    
  sprintf(salt_str, "%d", salt);
  hashed_password = _psy_lbm_hash_password(_password, salt);

  sqlite3_prepare_v2(_db, SQL_INSERT_USER, 
                     sizeof(SQL_INSERT_USER), &stmt, t);

  sqlite3_bind_text(stmt, 1, _username, strlen(_username), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, hashed_password, strlen(hashed_password), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, salt_str,  strlen(salt_str),  SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    perror("Problem inserting user row");
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
   
  u = psy_lbm_find_user_by_name(_db, _username);
  _psy_lbm_make_api_token_row(_db, u->id);
  psy_lbm_free_user(u);

}

void
psy_lbm_insert_bookmark(sqlite3* _db, uint32_t _user_id, char* _title,
  uint32_t _volume, uint32_t _chapter, uint32_t _page) {
  sqlite3_stmt* stmt = NULL;
  const char** t = NULL;
  
  sqlite3_prepare_v2(_db, SQL_INSERT_BOOKMARK,
                     sizeof(SQL_INSERT_BOOKMARK), &stmt, t);

  sqlite3_bind_int(stmt, 1, _user_id);
  sqlite3_bind_text(stmt, 2, _title, strlen(_title), SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, _volume);
  sqlite3_bind_int(stmt, 4, _chapter);
  sqlite3_bind_int(stmt, 5, _page);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    perror("Problem inserting bookmark");
  }

  sqlite3_finalize(stmt);
}

/** This should only be called once, from user creation */
void
_psy_lbm_make_api_token_row(sqlite3* _db, uint32_t _user_id) {
  sqlite3_stmt* stmt = NULL;
  const char** t = NULL;
  const char ntxt[] = "new";
  sqlite3_prepare_v2(_db, SQL_INSERT_API, sizeof(SQL_INSERT_API), &stmt, t);

  sqlite3_bind_int(stmt, 1, _user_id);
  sqlite3_bind_text(stmt, 2, ntxt, strlen(ntxt), SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    perror("Problem inserting api token");
  }

  sqlite3_finalize(stmt);
}


/** Search user by name. return 0 on not found, else 1 */
int 
_psy_lbm_user_exists(sqlite3* _db, char* _name) {
  user_t* u = psy_lbm_find_user_by_name(_db, _name);

  if (u == NULL) 
    return 0;

  psy_lbm_free_user(u);
  return 1;
}

/*
 * Thanks to Adam Lamers
 *   http://stackoverflow.com/questions/2262386/
 */
char*
_psy_lbm_hash_password(char* _pass, int _salt) {
  unsigned char hashed_pass[SHA256_DIGEST_LENGTH]; 
  char* ret_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);
  /* 10 is since epoch bytes for salt */ 
  char string[PSYLBM_PASSWORD_LENGTH + 10 + 1];
  char since_epoch[11];
  int i = 0;

  /* Build salted string */
  memset(string, 0, sizeof(string));
  sprintf(since_epoch, "%d", _salt);
  strcpy(string, _pass);
  strcat(string, since_epoch);

  /* And get the hashed value */
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Final(hashed_pass, &sha256);

  for (i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    sprintf((ret_string + (i * 2)), "%02x", hashed_pass[i]);
  }

  return ret_string;
}

char*
_psy_lbm_generate_token() {
  int buffsize = 124;
  char rand_data[buffsize];
  int max = buffsize;
  int i;
  unsigned char hashed[SHA256_DIGEST_LENGTH];
  char* ret_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1);

  /* Fill in a random string */
  for (i = 0; i < max; ++i) {
    rand_data[i] = rand() % 0xFF;
  }

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, rand_data, strlen(rand_data));
  SHA256_Final(hashed, &sha256);

  for (i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    sprintf((ret_string + (i * 2)), "%02x", hashed[i]);
  }

  return ret_string;
}
