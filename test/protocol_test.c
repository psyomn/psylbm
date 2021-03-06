#include "protocol.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

static int insert_message_type_test(void)
{
	const char *data = "ins|bookmark name|book-title|1|2|3|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_INSERT);
	return 0;
}

static int auth_message_type_test(void)
{
	const char *data = "auth|username|password";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_AUTH);
	return 0;
}

static int reg_message_type_test(void)
{
	const char *data = "reg|username|password";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_REGISTER);
	return 0;
}

static int del_message_type_test(void)
{
	const char *data = "del|bookmark-id|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_DELETE);
	return 0;
}

static int purge_message_type_test(void)
{
	const char *data = "purge|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_PURGE);
	return 0;
}

static int sync_message_type_test(void)
{
	const char *data = "sync|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_SYNC);
	return 0;
}

static int syncdata_message_type_test(void)
{
	const char *data = "syncdata|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);
	assert(recv_mess.type == MESS_SYNCDATA);
	return 0;
}

static int insert_message_test(void)
{
	const char *data = "ins|bookmark name|book-title|1|2|10|TOKEN";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(recv_mess.type == MESS_INSERT);
	assert(strcmp(recv_mess.message.insert.name, "bookmark name") == 0);
	assert(strcmp(recv_mess.message.insert.title, "book-title") == 0);
	assert(strcmp(recv_mess.message.insert.volume_str, "1") == 0);
	assert(strcmp(recv_mess.message.insert.chapter_str, "2") == 0);
	assert(strcmp(recv_mess.message.insert.page_str, "10") == 0);
	assert(strcmp(recv_mess.message.insert.token, "TOKEN") == 0);

	assert(recv_mess.message.insert.volume == 1);
	assert(recv_mess.message.insert.chapter == 2);
	assert(recv_mess.message.insert.page == 10);

	return 0;
}

static int update_message_test(void)
{
	const char *data = "ins|bookmark name|book-title|1|2|3|TOKEN|10";

	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(recv_mess.type == MESS_INSERT);
	assert(strcmp(recv_mess.message.insert.name, "bookmark name") == 0);
	assert(strcmp(recv_mess.message.insert.title, "book-title") == 0);
	assert(strcmp(recv_mess.message.insert.volume_str, "1") == 0);
	assert(strcmp(recv_mess.message.insert.chapter_str, "2") == 0);
	assert(strcmp(recv_mess.message.insert.page_str, "3") == 0);
	assert(strcmp(recv_mess.message.insert.token, "TOKEN") == 0);

	assert(recv_mess.message.insert.volume == 1);
	assert(recv_mess.message.insert.chapter == 2);
	assert(recv_mess.message.insert.page == 3);
	assert(recv_mess.message.insert.book_id == 10);

	return 0;
}

static int auth_message_test(void)
{
	const char *data = "auth|john.smith|password123";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.auth.username, "john.smith") == 0);
	assert(strcmp(recv_mess.message.auth.password, "password123") == 0);

	return 0;
}

static int register_message_test(void)
{
	const char *data = "reg|john.smith|password123";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.regist.username, "john.smith") == 0);
	assert(strcmp(recv_mess.message.regist.password, "password123") == 0);

	return 0;
}

static int delete_message_test(void)
{
	const char *data = "del|1234|abfed123ff";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.delete.bookmark_id_str, "1234") == 0);
	assert(strcmp(recv_mess.message.delete.token, "abfed123ff") == 0);
	assert(recv_mess.message.delete.bookmark_id == 1234);

	return 0;
}

static int purge_message_test(void)
{
	const char *data = "purge|defec8ed";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.purge.token, "defec8ed") == 0);

	return 0;
}

static int sync_message_test(void)
{
	const char *data = "sync|defec8edc007d00d";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.sync.token, "defec8edc007d00d") == 0);

	return 0;
}

static int syncdata_message_test(void)
{
	const char *data = "syncdata|defec8ed";
	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(data, &recv_mess);

	assert(strcmp(recv_mess.message.syncdata.token, "defec8ed") == 0);

	return 0;
}

int main(void)
{
	return 0
	       | insert_message_type_test()
	       | auth_message_type_test()
	       | reg_message_type_test()
	       | del_message_type_test()
	       | purge_message_type_test()
	       | sync_message_type_test()
	       | syncdata_message_type_test()

	       | update_message_test()
	       | insert_message_test()
	       | auth_message_test()
	       | register_message_test()
	       | delete_message_test()
	       | purge_message_test()
	       | sync_message_test()
	       | syncdata_message_test();
}
