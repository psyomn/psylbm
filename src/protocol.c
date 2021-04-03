#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

#include "protocol.h"

static int parse_insert_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_INSERT;
	const char *curr = &buffer[4];

	{       /* parse bookmark name */
		char *dst = &message->message.insert.name[0];
		while (*curr != '|') *(dst++) = *(curr++);
		curr++;
	}

	if (*curr == 0) goto error;

	{       /* parse book title */
		char *dst = &message->message.insert.title[0];
		while (*curr != '|') *(dst++) = *(curr++);
		curr++;
	}

	if (*curr == 0) goto error;

	{       /* parse volume */
		char *dst = &message->message.insert.volume_str[0];
		while (*curr != '|') *(dst++) = *(curr++);
		curr++;
	}

	if (*curr == 0) goto error;

	{       /* parse chapter */
		char *dst = &message->message.insert.chapter_str[0];
		while (*curr != '|') *(dst++) = *(curr++);
		curr++;
	}

	if (*curr == 0) goto error;

	{       /* parse page */
		char *dst = &message->message.insert.page_str[0];
		while (*curr != '|') *(dst++) = *(curr++);
		curr++;
	}

	if (*curr == 0) goto error;

	{       /* parse token */
		char *dst = &message->message.insert.token[0];
		while (*curr != 0 && *curr != '|') *(dst++) = *(curr++);
	}

	if (*curr != 0) {
		curr++; // get rid of the '|'

		/* there's one more entry -- parse */
		char *dst = &message->message.insert.book_id_str[0];
		while (*curr != 0) *(dst++) = *(curr++);
	}

	message->message.insert.volume = atoi(message->message.insert.volume_str);
	message->message.insert.chapter = atoi(message->message.insert.chapter_str);
	message->message.insert.page = atoi(message->message.insert.page_str);
	message->message.insert.book_id = atoi(message->message.insert.book_id_str);

	return 0;

error:
	return 1;
}

static int parse_auth_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_AUTH;
	const char *curr = &buffer[5];

	char *username = &message->message.auth.username[0];

	while (*curr != '|') *(username++) = *(curr++);
	curr++;

	char *password = &message->message.auth.password[0];

	while (*curr != 0) *(password++) = *(curr++);

	return 0;
}

static int parse_register_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_REGISTER;
	struct received_message recv_mess = { 0 };
	const size_t max_username_len = sizeof(recv_mess.message.regist.username) - 1;
	const char *curr = &buffer[4];

	char *username = &message->message.regist.username[0];
	size_t count = 0;

	while (*curr != '|') {
		if (count == max_username_len) break;

		*(username++) = *(curr++);
		count++;
	}
	curr++;

	char *password = &message->message.regist.password[0];

	while (*curr != 0) *(password++) = *(curr++);

	return 0;
}

static int parse_delete_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_DELETE;
	const char *curr = &buffer[4];

	char *bookmark_id_str = &message->message.delete.bookmark_id_str[0];

	while (*curr != '|') *(bookmark_id_str++) = *(curr++);
	curr++;

	char *token = &message->message.delete.token[0];

	while (*curr != 0) *(token++) = *(curr++);

	message->message.delete.bookmark_id = atoi(message->message.delete.bookmark_id_str);

	return 0;
}

static int parse_purge_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_PURGE;
	const char *curr = &buffer[6];

	char *token = &message->message.purge.token[0];

	while (*curr != 0) *(token++) = *(curr++);

	return 0;
}

static int parse_sync_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_SYNC;
	const char *curr = &buffer[5];

	char *token = &message->message.sync.token[0];

	while (*curr != 0) *(token++) = *(curr++);

	return 0;
}

static int parse_syncdata_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_SYNCDATA;
	const char *curr = &buffer[9];

	char *token = &message->message.syncdata.token[0];

	while (*curr != 0) *(token++) = *(curr++);

	return 0;
}

void psylbm_buffer_into_struct(const char *buffer, struct received_message *message)
{
	assert(buffer);
	assert(message);

	// TODO: this is for maintenance. In the future I'll rewrite the protocol
	//   to something smarter.

	if (strnlen(buffer, MAX_MESSAGE_SIZE) < 5)
		goto error;

	int ret = 0;

	if (strncmp(buffer, "ins", 3) == 0)
		ret = parse_insert_message(buffer, message);
	else if (strncmp(buffer, "aut", 3) == 0)
		ret = parse_auth_message(buffer, message);
	else if (strncmp(buffer, "reg", 3) == 0)
		ret = parse_register_message(buffer, message);
	else if (strncmp(buffer, "del", 3) == 0)
		ret = parse_delete_message(buffer, message);
	else if (strncmp(buffer, "pur", 3) == 0)
		ret = parse_purge_message(buffer, message);
	else if (strncmp(buffer, "syncd", 5) == 0)
		ret = parse_syncdata_message(buffer, message);
	else if (strncmp(buffer, "syn", 3) == 0)
		ret = parse_sync_message(buffer, message);
	else goto error;

	if (ret) goto error;

	return;

error:
	message->type = MESS_BADREQUEST;
	return;
}
