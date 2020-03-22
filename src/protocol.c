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
		while (*curr != '\0') *(dst++) = *(curr++);
		curr++;
	}

	message->message.insert.volume = atoi(message->message.insert.volume_str);
	message->message.insert.chapter = atoi(message->message.insert.chapter_str);
	message->message.insert.page = atoi(message->message.insert.page_str);

	return 0;

error:
	return 1;
}

static int parse_auth_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_AUTH;
	(void)buffer;
	return 0;
}

static int parse_register_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_REGISTER;
	(void)buffer;
	return 0;
}

static int parse_delete_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_DELETE;
	(void)buffer;
	return 0;
}

static int parse_purge_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_PURGE;
	(void)buffer;
	return 0;
}

static int parse_sync_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_SYNC;
	(void)buffer;
	return 0;
}

static int parse_syncdata_message(const char *buffer, struct received_message *message)
{
	message->type = MESS_SYNCDATA;
	(void)buffer;
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
