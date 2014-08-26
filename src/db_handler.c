#include <db_handler.h>
#include <common.h>
#include <sql_strings.h>
#include <psylbm.h>

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

