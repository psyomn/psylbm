#pragma once

#include "common.h"
#include "server.h"
#include "protocol.h"

int psylbm_handle_message(struct psylbm_server *, struct remote_host *, char *);
int psylbm_handle_authorization(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_insert(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_delete(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_register(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_token(struct psylbm_server *, struct remote_host *, uint32_t, char *);
int psylbm_handle_purge(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_sync(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_handle_syncdata(struct psylbm_server *, struct remote_host *, struct received_message *);
int psylbm_reply(struct psylbm_server *, struct remote_host *, const char *);
int psylbm_handle_error(const char *);
