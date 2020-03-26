#pragma once

#include <common.h>
#include <domain.h>
#include <db_token.h>
#include <db_bookmark.h>
#include <db_user.h>

#define PSYLBM_DB_NAME "psylbm-db.sqlite3"

void psylbm_check_db(void);
void psylbm_make_db(void);
