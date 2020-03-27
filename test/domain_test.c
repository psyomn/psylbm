#include "domain.h"

static int make_user_test(void)
{
	struct user *user = psylbm_make_user();

	psylbm_free_user(user);
	return 0;
}

static int make_bookmark_test(void)
{
	struct bookmark *bookmark = psylbm_make_bookmark();

	psylbm_free_bookmark(bookmark);
	return 0;
}

int main(void)
{
	return 0
	       | make_user_test()
	       | make_bookmark_test();
}
