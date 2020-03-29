#include "db_handler.h"
#include "common.h"
#include "sql_strings.h"
#include "psylbm.h"

/* Check if db exists - create if not */
void psylbm_check_db(const char *db_name)
{
	FILE *fd;

	// TODO: use stat
	fd = fopen(db_name, "r");

	if (!fd) {
		/* db dne, so make db */
		psylbm_make_db(db_name);
		return;
	}

	fclose(fd);
}

void psylbm_make_db(const char *db_name)
{
	sqlite3 *db = NULL;
	char *error = NULL;

	const char *tables[] = {
		SQL_CREATE_USERS,
		SQL_CREATE_BOOKMARKS,
		SQL_CREATE_API_TOKENS
	};
	const size_t sql_max = sizeof(tables) / sizeof(char *);

	printf("making database for the first time ... \n");
	sqlite3_open(db_name, &db);

	for (size_t i = 0; i < sql_max; ++i) {
		const int rc = sqlite3_exec(db, tables[i], NULL, 0, &error);

		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL ERROR: %s\n", error);
			sqlite3_free(error);
		}
	}
	sqlite3_close(db);
}
