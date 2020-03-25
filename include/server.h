#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdint.h>

#include <sqlite3.h>

#define PSYLBM_STARTED   1
#define PSYLBM_LISTENING 2
#define PSYLBM_PAUSED    3
#define PSYLBM_SHUTDOWN  4
#define PSYLBM_PANIC    -1

// TODO: should these be opaque?

struct psylbm_status_strings {
	int        id;
	const char *status;
};

struct psylbm_server {
	uint16_t portnum;
	char     hostname[255];
	int      status;
	sqlite3  *db;
	int      sock;
};

struct remote_host {
	socklen_t          l;
	struct sockaddr_in a;
};

struct psylbm_server *psylbm_make_server(uint16_t, const char *);
void psylbm_free_server(struct psylbm_server *);
void psylbm_server_listen(struct psylbm_server *);
void psylbm_print_server_info(struct psylbm_server *);
const char *psylbm_server_status(struct psylbm_server *);
