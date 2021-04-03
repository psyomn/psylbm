#include "common.h"
#include "server.h"
#include "db_handler.h"

#include <getopt.h>
#include <stdio.h>

void init_common(void)
{
	srand(time(NULL));
}

void init_server(void)
{
	psylbm_check_db(PSYLBM_DB_NAME);
}

void run_server(void)
{
	struct psylbm_server *server = NULL;

	server = psylbm_make_server(8080, "127.0.0.1");

	psylbm_print_server_info(server);
	psylbm_server_listen(server);

	psylbm_free_server(server);
}

void run_client(void)
{
	fprintf(stdout, "client code goes here\n");
}

void print_usage(const char *name)
{
	fprintf(stderr, "Usage: %s [-s] [-c <message>]\n",
		name);
}

int main(int argc, char *argv[])
{
	init_common();

	int opt = 0;

	while ((opt = getopt(argc, argv, "sc:")) != -1) {
		switch (opt) {
		case 's':
			init_server();
			run_server();
			break;
		case 'c':
			run_client();
			break;
		case 'h':
			print_usage(argv[0]);
			break;
		default:
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
