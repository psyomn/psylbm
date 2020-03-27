#include <domain.h>
#include <common.h>

struct user *psylbm_make_user(void)
{
	return calloc(1, sizeof(struct user));
}

void psylbm_free_user(struct user *user)
{
	if (user == NULL) return;
	if (user->name != NULL) free(user->name);
	if (user->password != NULL) free(user->password);
	if (user->salt != NULL) free(user->salt);

	free(user);
}

struct bookmark *psylbm_make_bookmark(void)
{
	return calloc(1, sizeof(struct bookmark));
}

void psylbm_free_bookmark(struct bookmark *bookmark)
{
	if (bookmark == NULL) return;
	if (bookmark->name != NULL) free(bookmark->name);
	if (bookmark->title != NULL) free(bookmark->title);

	free(bookmark);
}
