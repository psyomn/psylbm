#ifndef SQL_STRINGS_H
#define SQL_STRINGS_H

/* Table creation sql */
#define SQL_CREATE_USERS                         \
	"CREATE TABLE users("                          \
	"id       INTEGER PRIMARY KEY AUTOINCREMENT, " \
	"username TEXT UNIQUE, "                       \
	"password TEXT, "                              \
	"salt     TEXT);"

#define SQL_CREATE_BOOKMARKS                    \
	"create table bookmarks("                     \
	"id      INTEGER PRIMARY KEY AUTOINCREMENT, " \
	"user_id INTEGER,"                            \
	"name    TEXT UNIQUE,"                        \
	"title   TEXT,"                               \
	"volume  INTEGER,"                            \
	"chapter INTEGER,"                            \
	"page    INTEGER);"

#define SQL_CREATE_API_TOKENS \
	"CREATE TABLE api_tokens("  \
	"user_id INTEGER,"          \
	"token TEXT);"

/* Inserts */
#define SQL_INSERT_USER                           \
	"INSERT INTO users (username, password, salt) " \
	"VALUES (?,?,?);"

#define SQL_INSERT_API \
	"INSERT INTO api_tokens (user_id, token) VALUES (?,?)"

#define SQL_UPDATE_TOKEN \
	"UPDATE api_tokens set token=? where user_id=?"

#define SQL_INSERT_BOOKMARK                  \
	"INSERT INTO bookmarks "                   \
	"(user_id,name,title,volume,chapter,page)" \
	" VALUES (?,?,?,?,?,?);"

#define SQL_UPDATE_BOOKMARK_VALUES \
	"UPDATE bookmarks SET "          \
	"name    = ? , "                 \
	"title   = ? , "                 \
	"volume  = ? , "                 \
	"chapter = ? , "                 \
	"page    = ? "                   \
	"WHERE id = ?;"

/* Find */
#define SQL_FIND_USER_BY_ID \
	"SELECT * FROM users WHERE id = ?;"

#define SQL_FIND_USER_BY_NAME \
	"SELECT * FROM users WHERE username = ?;"

#define SQL_FIND_BOOKMARKS_BY_USER_ID \
	"SELECT * FROM bookmarks WHERE user_id = ?;"

#define SQL_FIND_BOOKMARK_BY_NAME \
	"SELECT * FROM bookmarks WHERE name = ?;"

#define SQL_FIND_BOOKMARK \
	"SELECT * FROM bookmarks WHERE id = ?;"

#define SQL_FIND_USER_BY_TOKEN \
	"SELECT user_id FROM api_tokens WHERE token = ?;"

#define SQL_DELETE_BOOKMARK \
	"DELETE FROM bookmarks WHERE id = ?;"

#define SQL_PURGE_BOOKMARKS \
	"DELETE FROM bookmarks WHERE user_id = ?;"

/* Count */
#define SQL_COUNT_USER_BOOKMARKS \
	"SELECT COUNT(*) FROM bookmarks WHERE user_id = ?;"

#endif // ifndef SQL_STRINGS_H
