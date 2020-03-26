#include "common.h"
#include "server.h"
#include "db_handler.h"

void init(void)
{
	srand(time(NULL));
	psylbm_check_db();
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	init();

	struct psylbm_server *server = NULL;

	server = psylbm_make_server(8080, "127.0.0.1");

	psylbm_print_server_info(server);
	psylbm_server_listen(server);

	psylbm_free_server(server);

	return 0;
}
