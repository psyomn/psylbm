#include <domain.h>
#include <common.h>

user_t *psy_lbm_make_user(void)
{
	user_t *user = calloc(0, sizeof(user_t));

	return user;
}

void psy_lbm_free_user(user_t *user)
{
	if (user == NULL) return;
	if (user->name != NULL) free(user->name);
	if (user->password != NULL) free(user->password);
	if (user->salt != NULL) free(user->salt);

	free(user);
}

void psy_lbm_free_bookmark(bookmark_t *bookmark)
{
	if (bookmark == NULL) return;
	if (bookmark->name != NULL) free(bookmark->name);
	if (bookmark->title != NULL) free(bookmark->title);
	free(bookmark);
}

bookmark_t *psy_lbm_make_bookmark(void)
{
	bookmark_t *b = calloc(0, sizeof(bookmark_t));

	return b;
}
