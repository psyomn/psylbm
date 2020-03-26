#pragma once

#include <common.h>

struct user {
	uint32_t id;
	char     *name;
	char     *password;
	char     *salt;
};

struct bookmark {
	uint32_t id;
	uint32_t user_id;
	char     *name;
	char     *title;
	uint32_t volume;
	uint32_t chapter;
	uint32_t page;
};

struct api_token {
	uint32_t user_id;
	char     *token;
};

struct user *psylbm_make_user(void);
void psylbm_free_user(struct user *);
struct bookmark *psylbm_make_bookmark(void);
void psylbm_free_bookmark(struct bookmark *);
