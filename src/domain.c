#include <domain.h>
#include <common.h>

void
psy_lbm_free_user(user_t* u){ 
  if (u == NULL) return;

  if (u->name     != NULL) {
    free(u->name);
  }
  if (u->password != NULL) {
    free(u->password);
  }
  if (u->salt     != NULL) {
    free(u->salt);
  }

  free(u);
}

user_t*
psy_lbm_make_user() {
  user_t* u = malloc(sizeof(user_t));
  u->id       = 0;
  u->name     = NULL;
  u->password = NULL;
  u->salt     = NULL;
  return u;
}
