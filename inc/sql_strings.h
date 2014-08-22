#ifndef SQL_STRINGS_H
#define SQL_STRINGS_H

/* Table creation sql */
#define SQL_CREATE_USERS \
  "CREATE TABLE users("\
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "\
    "username TEXT UNIQUE, password TEXT);"

#define SQL_CREATE_BOOKMARKS \
  "create table bookmarks("\
     "id INTEGER PRIMARY KEY AUTOINCREMENT, "\
     "user_id INTEGER,"\
     "title TEXT,"\
     "volume INTEGER,"\
     "chapter INTEGER,"\
     "page INTEGER);"\

#define SQL_CREATE_API_TOKENS \
  "CREATE TABLE api_tokens("\
    "user_id INTEGER,"\
    "token TEXT);"

/* Inserts */
#define SQL_INSERT_USER \
  "INSERT INTO users (username, password) "\
  "VALUES (?,?);"

#define SQL_INSERT_API \
  "INSERT INTO api_tokens (user_id, token) VALUES (?,?)"

#define SQL_INSERT_BOOKMARK \
  "INSERT INTO bookmarks (user_id,title,volume,chapter,page)"\
  " VALUES (?,?,?,?,?);"
 
/* Find */
#define SQL_FIND_USER_BY_ID \
  "SELECT FROM users WHERE id = ?;"

#define SQL_FIND_USER_BY_NAME \
  "SELECT FROM users WHERE username = 'jon';"

#define SQL_FIND_BOOKMARKS_BY_USER_ID \
  "SELECT FROM bookmarks WHERE user_id = ?;"

#endif
