#ifndef DOMAIN_H
#define DOMAIN_H

#include <common.h>

/* Domain structures that are representations of tha table schemass */

struct user {
	uint32_t	id;
	char *		name;
	char *		password;
	char *		salt;
};
typedef struct user user_t;

struct bookmark {
	uint32_t	id;
	uint32_t	user_id;
	char *		name;
	char *		title;
	uint32_t	volume;
	uint32_t	chapter;
	uint32_t	page;
};
typedef struct bookmark bookmark_t;

struct api_token {
	uint32_t	user_id;
	char *		token;
};
typedef struct api_token api_token_t;

struct remote_host {
	socklen_t		l;
	struct sockaddr_in	a;
};
typedef struct remote_host remote_host_t;

void      psy_lbm_free_user(user_t *);
user_t *psy_lbm_make_user();
bookmark_t *psy_lbm_make_bookmark();
void      psy_lbm_free_bookmark(bookmark_t *);

#endif // ifndef DOMAIN_H
