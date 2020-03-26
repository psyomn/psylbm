#pragma once
#include <stdint.h>

enum { MAX_MESSAGE_SIZE = (1 << 16) - (8 + 20) };

enum message_type { START_MESSAGE_TYPE = 0, MESS_AUTH = 0,
		    MESS_INSERT, MESS_REGISTER, MESS_DELETE,
		    MESS_PURGE, MESS_SYNC, MESS_SYNCDATA,
		    MESS_BADREQUEST, END_MESSAGE_TYPE };

struct auth_message {
	char username[51];
	char password[255];
};

struct insert_message {
	char     name[255];
	char     title[255];
	char     volume_str[255];
	char     chapter_str[255];
	char     page_str[255];
	char     token[255];
	char     book_id_str[255];

	uint32_t volume;
	uint32_t chapter;
	uint32_t page;
	uint32_t book_id;

	uint8_t  type; // TODO: is this used?
	uint32_t user_id;
};

struct register_message {
	char username[255];
	char password[255];
};

struct delete_message {
	char     token[255];
	char     bookmark_id_str[255];
	uint64_t bookmark_id;
};

struct purge_message {
	char token[255];
};

struct sync_message {
	char token[255];
};

struct syncdata_message {
	char token[255];
};

struct received_message {
	uint8_t type;
	union {
		struct insert_message   insert;
		struct register_message regist;
		struct delete_message delete;
		struct purge_message    purge;
		struct sync_message     sync;
		struct syncdata_message syncdata;
		struct auth_message     auth;
	} message;
};



void psylbm_buffer_into_struct(const char *, struct received_message *);
enum message_type psylbm_message_type(const struct received_message *);
