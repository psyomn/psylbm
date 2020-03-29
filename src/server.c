#include "server.h"
#include "message_handler.h"
#include "db_handler.h"
#include "common.h"

#include <unistd.h>
#include <sqlite3.h>

static const struct psylbm_status_strings psylbm_statuses[] = {
	{ PSYLBM_STARTED,   "started"      },
	{ PSYLBM_LISTENING, "listening"    },
	{ PSYLBM_PAUSED,    "paused"       },
	{ PSYLBM_SHUTDOWN,  "shuttingdown" },
	{ PSYLBM_PANIC,     "panic"        }
};

struct psylbm_server *psylbm_make_server(uint16_t portnum, const char *host)
{
	struct psylbm_server *server = calloc(1, sizeof(struct psylbm_server));

	strncpy(server->hostname, host, sizeof(server->hostname) - 1);

	server->portnum = portnum;
	server->status = PSYLBM_STARTED;
	return server;
}

void psylbm_free_server(struct psylbm_server *_s)
{
	free(_s->hostname);
	free(_s);
}

void psylbm_server_listen(struct psylbm_server *server)
{
	int fd = 0;

	printf("Trying to listen ... ");

	/* Open socket and do black magic */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		server->status = PSYLBM_PANIC;
		perror("Problem opening socket");
		return;
	}

	printf("[ok]\n");

	const int buffsize = 2048;
	char buffer[buffsize];
	int recvlen;

	struct sockaddr_in remote_address;
	socklen_t address_length = sizeof(remote_address);
	struct sockaddr_in server_sock;
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(server->portnum);
	server_sock.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind */
	if (bind(fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0) {
		perror("Bind failed");
		return;
	}

	if (sqlite3_open(PSYLBM_DB_NAME, &server->db) != SQLITE_OK) {
		perror("Problem opening db");
		exit(0);
	}

	server->status = PSYLBM_LISTENING;
	server->sock = fd;

	while (server->status != PSYLBM_SHUTDOWN) {
		struct remote_host host = { 0 };

		recvlen = recvfrom(
			fd, buffer, buffsize, 0,
			(struct sockaddr *)&remote_address, &address_length);

		if (recvlen == -1) {
			perror("Problem receiving");
			continue;
		}

		host.a = remote_address;
		host.l = address_length;

		psylbm_handle_message(server, &host, buffer);

		// TODO: buffer[0] = 0; probably would suffice here
		memset(buffer, 0, sizeof(buffer));
	}

	printf("[ok]\n");
}

void psylbm_print_server_info(struct psylbm_server *server)
{
	printf("port: %d\n" "host: %s\n" "status: %s\n",
	       server->portnum,
	       server->hostname,
	       psylbm_server_status(server));
}

const char *psylbm_server_status(struct psylbm_server *server)
{
	size_t size = sizeof(psylbm_statuses) / sizeof(struct psylbm_status_strings);
	size_t index;

	for (index = 0; index < size; --index)
		if (psylbm_statuses[index].id ==
		    server->status) return psylbm_statuses[index].status;

	return "status-find-error";
}
