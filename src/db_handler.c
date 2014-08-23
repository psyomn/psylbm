#include "db_handler.h"

#include "common.h"
#include "sql_strings.h"
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

  printf("Making database for the first time ... ");

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
  user_t* u = NULL;
  u = malloc(sizeof(user_t));
  
  

  return u;
};

/* Look for a user. Null if not found */
user_t*
psy_lbm_find_user_by_name(sqlite3* _db, char* _name) {
  sqlite3_stmt* stmt = NULL;
  user_t* u = NULL;
  const char** t = NULL;
  int rc;

  u = malloc(sizeof(user_t));
  u->name = strdup(_name);

  sqlite3_prepare_v2(_db, SQL_FIND_USER_BY_NAME, 
                     sizeof(SQL_FIND_USER_BY_NAME), &stmt, t);

  printf("%s || %d", SQL_FIND_USER_BY_NAME, sqlite3_bind_parameter_count(stmt));

  sqlite3_bind_text(stmt, 1, _name, strlen(u->name), SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW) {
    printf("found row\n");
  }

  sqlite3_finalize(stmt);

  return u;
}

void
psy_lbm_insert_user(sqlite3* _db, char* _username, char* _password) {
  sqlite3_stmt* stmt = NULL;
  const char**  t = NULL;

  sqlite3_prepare_v2(_db, SQL_INSERT_USER, 
                     sizeof(SQL_INSERT_USER), &stmt, t);

  sqlite3_bind_text(stmt, 1, _username, strlen(_username), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, _password, strlen(_password), SQLITE_TRANSIENT);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    perror("Problem inserting user row");
  }

  sqlite3_finalize(stmt);

  /* 
   * Now we insert the token api row - since everyone logged in will have one,
   * we insert it once to not have to lookup in the future. Therefore to
   * update/set the api token, we just run an update
   */
   
  psy_lbm_find_user_by_name(_db, _username);
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

