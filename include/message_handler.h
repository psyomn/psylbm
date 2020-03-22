#pragma once

#include <common.h>

#include <server.h>
#include <domain.h>

void _psy_lbm_reply(psy_lbm_server_t *, remote_host_t *, const char *);

void psy_lbm_handle_message(psy_lbm_server_t *, remote_host_t *, char *);

void psy_lbm_handle_authorization(psy_lbm_server_t *, remote_host_t *, char *, char *);

int psy_lbm_handle_insert(
	psy_lbm_server_t *,
	remote_host_t *,

	char *, char *, uint32_t,
	uint32_t, uint32_t, char *,
	uint32_t);

int psy_lbm_handle_delete(
	psy_lbm_server_t *,
	remote_host_t *,
	uint32_t,
	char *
	);

int psy_lbm_handle_register(psy_lbm_server_t *, remote_host_t *, char *, char *);

int psy_lbm_handle_token(
	psy_lbm_server_t *,
	remote_host_t *,
	uint32_t,
	char *);

int psy_lbm_handle_purge(psy_lbm_server_t *, remote_host_t *, char *);

int psy_lbm_handle_sync(psy_lbm_server_t *, remote_host_t *, char *);

int psy_lbm_handle_error(const char *);

int psy_lbm_handle_syncdata(psy_lbm_server_t *, remote_host_t *, char *);
