#include <domain.h>
#include <common.h>

void
psy_lbm_free_user(user_t* u){ 
  if(u->name)     free(u->name);
  if(u->password) free(u->password);
  if(u->salt)     free(u->salt);
  free(u);
}

user_t*
psy_lbm_make_user() {
  user_t* u = malloc(sizeof(user_t));
  u->name     = NULL;
  u->password = NULL;
  u->salt     = NULL;
  return u;
}
