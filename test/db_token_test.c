#include "db_token.h"

#include <assert.h>
#include <string.h>

static int generate_token_test(void)
{
	char *token = psylbm_generate_token();
	const size_t len = strnlen(token, SHA256_DIGEST_LENGTH * 4);

	assert(len == 64);

	free(token);
	return 0;
}

int main(void)
{
	return generate_token_test();
}
