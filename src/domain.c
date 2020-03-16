#include <domain.h>
#include <common.h>

void psy_lbm_free_user(user_t *u)
{
	if (u == NULL) return;

	if (u->name != NULL)
		free(u->name);

	if (u->password != NULL)
		free(u->password);

	if (u->salt != NULL)
		free(u->salt);

	free(u);
}

user_t *psy_lbm_make_user()
{
	user_t *u = malloc(sizeof(user_t));

	u->id = 0;
	u->name = NULL;
	u->password = NULL;
	u->salt = NULL;
	return u;
}

void psy_lbm_free_bookmark(bookmark_t *_b)
{
	if (_b == NULL) return;

	if (_b->name != NULL) free(_b->name);

	if (_b->title != NULL) free(_b->title);
	free(_b);
}

bookmark_t *psy_lbm_make_bookmark()
{
	bookmark_t *b = malloc(sizeof(bookmark_t));

	b->id = 0;
	b->user_id = 0;
	b->name = NULL;
	b->title = NULL;
	b->volume = 0;
	b->chapter = 0;
	b->page = 0;
	return b;
}
