#include "common.h"

char *psylbm_strndup(const unsigned char *src, size_t len)
{
	// sqlite3 does not store length for the 0 termination of
	// strings.
	const size_t c_str_len = len + 1;

	char *ret = calloc(1, sizeof(char) * c_str_len);

	// TODO: this is bad because UTF. FIXME
	memcpy(ret, src, len);

	ret[len] = 0;

	return ret;
}
