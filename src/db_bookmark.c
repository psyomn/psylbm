#include <db_bookmark.h>
#include <sql_strings.h>

int
psy_lbm_insert_bookmark(sqlite3* _db, uint32_t _user_id, char* _name, char* _title,
  uint32_t _volume, uint32_t _chapter, uint32_t _page) {
  sqlite3_stmt* stmt = NULL;
  const char** t = NULL;
  
  sqlite3_prepare_v2(_db, SQL_INSERT_BOOKMARK,
    sizeof(SQL_INSERT_BOOKMARK), &stmt, t);

  sqlite3_bind_int(stmt,  1, _user_id);
  sqlite3_bind_text(stmt, 2, _name,  strlen(_name), SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, _title, strlen(_title), SQLITE_STATIC);
  sqlite3_bind_int(stmt,  4, _volume);
  sqlite3_bind_int(stmt,  5, _chapter);
  sqlite3_bind_int(stmt,  6, _page);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    printf("Problem inserting bookmark [%d] [%s] [%s]\n", _user_id, _name, _title);
    return -1;
  }

  sqlite3_finalize(stmt);
  return 0;
}

bookmark_t*
psy_lbm_find_bookmark_by_name(sqlite3* _db, char* _name) {
  bookmark_t* book = NULL;
  const char** t = NULL;
  sqlite3_stmt* stmt = NULL;
  
  sqlite3_prepare_v2(_db, SQL_FIND_BOOKMARK_BY_NAME,
    sizeof(SQL_FIND_BOOKMARK_BY_NAME), &stmt, t);

  sqlite3_bind_text(stmt, 1, _name, strlen(_name), SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    printf("Problem finding bookmark [%s]\n", _name);
    return NULL;
  }

  book          = psy_lbm_make_bookmark();
  book->id      = sqlite3_column_int(stmt, 0);
  book->user_id = sqlite3_column_int(stmt, 1);
  book->name    = strdup((char*) sqlite3_column_text(stmt, 2));
  book->title   = strdup((char*) sqlite3_column_text(stmt, 3));
  book->volume  = sqlite3_column_int(stmt, 4);
  book->chapter = sqlite3_column_int(stmt, 5);
  book->page    = sqlite3_column_int(stmt, 6);

  sqlite3_finalize(stmt);

  return book;
}

