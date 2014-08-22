#ifndef SQL_STRINGS_H
#define SQL_STRINGS_H

/* Table creation sql */
#define SQL_CREATE_USERS \
  "create table users("\
    "id integer primary key autoincrement, "\
    "username text unique, password text);"

#define SQL_CREATE_BOOKMARKS \
  "create table bookmarks("\
     "id integer primary key autoincrement, "\
     "user_id integer,"\
     "title text,"\
     "volume integer,"\
     "chapter integer,"\
     "page integer);"\

#define SQL_CREATE_API_TOKENS \
  "create table api_tokens("\
    "user_id integer,"\
    "assigned_token text);"

/* Inserts */
#define SQL_INSERT_USER \
  "insert into users (username, password) "\
  "values (?, ?);"

#define SQL_INSERT_BOOKMARK \
  "insert into bookmarks (user_id,title,volume,chapter,page)"\
  " values (?,?,?,?,?);"
 
#define SQL_FIND_USER_BY_ID \
  "select from users where users.user_id = ?"

#define SQL_FIND_USER_BY_NAME \
  "select from users where users.name = ?"

#define SQL_FIND_BOOKMARKS_BY_USER_ID \
  "select from bookmarks where bookmarks.user_id = ?"
  

#endif

