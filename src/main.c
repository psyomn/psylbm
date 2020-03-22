#include "common.h"
#include "server.h"
#include "db_handler.h"

void init(void)
{
	srand(time(NULL));
	psy_lbm_check_db();
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	init();

	psy_lbm_server_t *server = NULL;

	server = psy_lbm_make_server(8080, "127.0.0.1");

	psy_lbm_print_server_info(server);
	psy_lbm_server_listen(server);

	psy_lbm_free_server(server);

	return 0;
}
