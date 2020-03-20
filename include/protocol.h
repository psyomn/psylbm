#pragma once
#include <stdint.h>

enum { MAX_MESSAGE_SIZE = (1 << 16) - (8 + 20) };

enum message_type { START_MESSAGE_TYPE = 0, MESS_AUTH = 0,
		    MESS_INSERT, MESS_REGISTER, MESS_DELETE,
		    MESS_PURGE, MESS_SYNC, MESS_SYNCDATA,
		    MESS_BADREQUEST, END_MESSAGE_TYPE };

struct insert_message {
	char		name[255];
	char		title[255];
	char		volume_str[255];
	char		chapter_str[255];
	char		page_str[255];
	char		token[255];
	char		book_id_str[255];

	uint32_t	volume;
	uint32_t	chapter;
	uint32_t	page;
	uint32_t	book_id;

	uint8_t		type;
};

struct register_message {
};

struct delete_message {
};

struct purge_message {
};

struct sync_message {
};

struct syncdata_message {
};

struct badrequest_message {
};

struct received_message {
	uint8_t type;
	union {
		struct insert_message		insert;
		struct register_message		regist;
		struct delete_message delete;
		struct purge_message		purge;
		struct sync_message		sync;
		struct syncdata_message		syncdata;
		struct badrequest_message	badrequest;
	} message;
};



void psylbm_buffer_into_struct(const char *, struct received_message *);
enum message_type psylbm_message_type(const struct received_message *);
