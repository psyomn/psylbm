#include "common.h"

#include "psylbm.h"
#include "domain.h"
#include "db_handler.h"
#include "message_handler.h"
#include "protocol_responses.h"
#include "sql_strings.h"
#include "protocol.h"

int psylbm_reply(struct psylbm_server *server, struct remote_host *host, const char *message)
{
	// TODO find better estimate here.
	const size_t message_len = strnlen(message, 500);

	return sendto(server->sock, message, message_len, 0,
		      (struct sockaddr *)&host->a, host->l);
}

int psylbm_handle_message(struct psylbm_server *_s, struct remote_host *_h, char *message)
{
	char delimiters[] = "|";
	char *token;
	int badrequest = 0;

	struct received_message recv_mess = { 0 };

	psylbm_buffer_into_struct(message, &recv_mess);

	switch (recv_mess.type) {
	default: goto error;
	case MESS_AUTH:
		psylbm_handle_authorization(_s, _h, user, pass);
		break;
	case MESS_INSERT:
		psylbm_handle_insert(_s, _h, name, title, i_vol, i_chap, i_page,
				     auth_token, i_bkid);
		break;
	case MESS_REGISTER:
		psylbm_handle_register(_s, _h, user, pass);
		break;
	case MESS_DELETE:
		psylbm_handle_delete(_s, _h, i_bkid, token);
		break;
	case MESS_PURGE:
		psylbm_handle_purge(_s, _h, auth_token);
		break;
	case MESS_SYNC:
		psylbm_handle_sync(_s, _h, auth_token);
		break;
	case MESS_SYNCDATA:
		psylbm_handle_syncdata(_s, _h, auth_token);
		break;
	}

	return 0;

error:
	fprintf(stdout, "Received malformed request [%s]\n", message);
	psylbm_reply(_s, _h, PSYLBM_BAD_REQUEST);
	return -1;
}

/* provided a username and password generate a token, and send it back */
int psylbm_handle_authorization(struct psylbm_server *_s, struct remote_host *_h,
				char *_username, char *_password)
{
	char *hash = NULL;
	int salt;
	user_t *u = psylbm_find_user_by_name(_s->db, _username);

	if (u == NULL) {
		/* No user found - send back fail message */
		_psylbm_reply(_s, _h, PSYLBM_AUTH_FAIL);
		return 1;
	}

	salt = atoi(u->salt);
	hash = _psylbm_hash_password(_password, salt);

	if (!strcmp(hash, u->password)) {
		char *token = _psylbm_generate_token();
		psylbm_handle_token(_s, _h, u->id, token);
		printf("Authenticate [%s]-[%s]\n", u->name, token);
		free(token);
	} else {
		/* TODO Log me */
		printf("Failed login attempt [%s]\n", u->name);
		_psylbm_reply(_s, _h, PSYLBM_AUTH_FAIL);
	}

	free(hash);
	psylbm_free_user(u);

	return 0;
}

/* Send the book info (title, vol, chapter, page), and if the token is a valid
 * login, then process the insert. The insert looks up if a same book has
 * previously been inserted. If that is the case, the info is updated. If not, a
 * new record is created.
 */
int psylbm_handle_insert(struct psylbm_server *server, struct remote_host *host,
			 char *_name, char *_title, uint32_t _vol,
			 uint32_t _chapter, uint32_t _page, char *token,
			 uint32_t _bookmark_id)
{
	int ret = 0;
	uint8_t error = 0;
	uint32_t uid = psylbm_find_user_id_by_token(server->db, token, &error);
	bookmark_t *bm = NULL;

	/* No such user */
	if (error) {
		psylbm_reply(server, host, PSYLBM_BAD_REQUEST);
		return -1;
	}

	if (_bookmark_id == 0)
		/* No id given - Does the previous bookmark exist? */
		bm = psylbm_find_bookmark_by_name(server->db, _name);
	else
		bm = psylbm_find_bookmark(server->db, _bookmark_id);

	if (bm == NULL) {
		/* No such bookmark - inset */
		ret = psylbm_insert_bookmark(
			server->db, uid, _name, _title, _vol, _chapter, _page);
		bm = psylbm_find_bookmark_by_name(server->db, _name);
	} else {
		/* Found bookmark - update */
		ret = psylbm_update_bookmark(
			server->db, _name, _title, _vol, _chapter, _page, _bookmark_id);
	}

	if (!ret) {
		char reply[128] = { 0 };
		char idstr[64];
		sprintf(idstr, "%d", bm->id);
		strcpy(reply, PSYLBM_INS_OK);
		strcat(reply, idstr);
		psylbm_reply(server, host, reply);
	} else {
		psylbm_reply(server, host, PSYLBM_INS_FAIL);
	}

	psylbm_free_bookmark(bm);

	return 0;
}

int psylbm_handle_delete(struct psylbm_server *server, struct remote_host *host,
			 struct received_message *recv_mess)
{
	struct bookmark *bm = NULL;
	const uint32_t book_id = recv_mess->message.delete.bookmark_id;
	const char *token = &recv_mess->message.delete.token[0];
	uint32_t user_id = 0;
	uint8_t error = 0;
	int ret = 0;

	user_id = psylbm_find_user_id_by_token(server->db, token, &error);
	bm = psylbm_find_bookmark(server->db, book_id);

	if (bm && (bm->user_id == user_id)) {
		/* Authorization ok */
		printf("Deleting...\n");

		if (psylbm_delete_bookmark(server->db, book_id) == 0)
			psylbm_reply(server, host, PSYLBM_DEL_OK);
		else
			psylbm_reply(server, host, PSYLBM_DEL_FAIL);
	} else if (!bm) {
		/* TODO log me */
		ret = -1;
		printf("Tried to delete non-existant bookmark\n");
	} else {
		ret = -1;
		printf("Unauthorized delete request\n"); /* TODO this should be logged */
	}

	if (ret != 0)
		psylbm_reply(server, host, PSYLBM_DEL_FAIL);

	psylbm_free_bookmark(bm);

	return ret;
}

/*
 * Register an account. Return a status int.
 */
int psylbm_handle_register(struct psylbm_server *server, struct remote_host *host,
			   struct received_message *recv_mess)
{
	const char *user = &recv_mess->message.regist.username[0];
	const char *pass = &recv_mess->message.regist.password[0];
	const int ret = psylbm_insert_user(server->db, user, pass);

	if (ret == -1) {
		psylbm_reply(server, host, PSYLBM_SERVER_ERROR);
	} else if (ret == -2) {
		_psylbm_reply(server, host, PSYLBM_USERNAME_TAKEN);
		printf("--| Username [%s] has been taken.\n", user); /* TODO: logme */
	} else {
	}

	return ret;
}

int psylbm_handle_token(struct psylbm_server *server, struct remote_host *host,
			uint32_t user_id, char *token)
{
	int ret = psylbm_set_token(server->db, user_id, token);

	if (!ret) {
		psylbm_set_token(server->db, user_id, token);

		char resp[100];
		memset(resp, 0, sizeof(resp));
		strcpy(resp, PSYLBM_AUTH);
		strcat(resp, token);
		psylbm_reply(server, host, resp);
	} else {
		psylbm_reply(server, host, PSYLBM_SERVER_ERROR);
	}

	return 0;
}

int psylbm_handle_purge(struct psylbm_server *server, struct remote_host *host,
			struct received_message *recv_mess)
{
	char *token = &recv_mess->message.purge.token[0];
	int ret = 0;
	uint8_t error = 0;
	uint32_t user_id = psylbm_find_user_id_by_token(server->db, token, &error);

	if (error) {
		psylbm_reply(server, host, PSYLBM_PURGE_FAIL);
		return -1;
	}

	psylbm_purge_bookmarks(server->db, user_id);
	psylbm_reply(server, host, PSYLBM_PURGE_OK);

	return ret;
}

int psylbm_handle_sync(struct psylbm_server *server, struct remote_host *host,
		       struct received_message *recv_mess)
{
	char *token = &recv_mess->message.sync.token[0];
	uint8_t error = 0;
	uint32_t user_id = psylbm_find_user_id_by_token(server->db, token, &error);

	if (error) {
		printf("Problem syncing - no such user...\n");
		psylbm_reply(server, host, PSYLBM_SYNC_FAIL);
		return -1;
	}

	int ret = 0;
	uint32_t count = 0;
	char reply[128] = { 0 };
	char countstr[64] = { 0 };

	count = psylbm_count_user_bookmarks(server->db, user_id);

	sprintf(countstr, "%d", count);
	strcpy(reply, PSYLBM_SYNC);
	strcat(reply, countstr);
	psylbm_reply(server, host, reply);

	return ret;
}

/**
 * Usually it's better to separate db calls to another function, but for the
 * sake of simplicity, and not relying on quite a bit of memory, we'll bundle
 * the row-iteration on this function
 */
int psylbm_handle_syncdata(struct psylbm_server *server, struct remote_host *host,
			   struct received_message *recv_mess)
{
	char *token = &recv_mess->message.syncdata.token[0];
	int ret = 0;
	uint8_t error = 0;
	uint32_t user_id = psylbm_find_user_id_by_token(server->db, token, &error);

	if (error) {
		printf("Syncdata error: no such user [%d]\n", user_id);
		psylbm_reply(server, host, PSYLBM_SYNCDATA_FAIL);
		return -1;
	}

	struct bookmark *book = NULL;
	sqlite3_stmt *stmt = NULL;
	const char **t = NULL;
	char reply[1024] = { 0 };

	book = psylbm_make_bookmark();

	sqlite3_prepare_v2(server->db, SQL_FIND_BOOKMARKS_BY_USER_ID,
			   sizeof(SQL_FIND_BOOKMARKS_BY_USER_ID), &stmt, t);

	sqlite3_bind_int(stmt, 1, user_id);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		// TODO: strings are weird in sqlite3. Also take a look at <common.h>
		const unsigned char *bookmark_name_uc = sqlite3_column_text(stmt, 2);
		int bookmark_name_uc_size = sqlite3_column_bytes(stmt, 2);
		char *converted_bookmark_name = psylbm_strndup(bookmark_name_uc, bookmark_name_uc_size);

		const unsigned char *title_uc = sqlite3_column_text(stmt, 3);
		int title_uc_size = sqlite3_column_bytes(stmt, 3);
		char *converted_title = psylbm_strndup(title_uc, title_uc_size);

		book->id = sqlite3_column_int(stmt, 0);
		book->user_id = sqlite3_column_int(stmt, 1);
		book->name = converted_bookmark_name;
		book->title = converted_title;
		book->volume = sqlite3_column_int(stmt, 4);
		book->chapter = sqlite3_column_int(stmt, 5);
		book->page = sqlite3_column_int(stmt, 6);

		printf("Sync bookmark with id [%d] for [%s]\n", book->id, token);

		memset(reply, 0, sizeof(reply));
		sprintf(reply, "%d|%s|%s|%d|%d|%d",
			book->id, book->name, book->title, book->volume,
			book->chapter, book->page);
		_psylbm_reply(server, host, reply);
	}

	/* Since we're not dupping in this case, above */
	book->name = NULL;
	book->title = NULL;

	psylbm_free_bookmark(book);
	sqlite3_finalize(stmt);
	printf("Done with syncdata...\n");
	return ret;
}

/* TODO In case we ever want to log, this should handle the error */
int psylbm_handle_error(const char *_sent_stuff)
{
	printf("error: %s\n", _sent_stuff);
	return 0;
}
