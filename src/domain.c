#include <domain.h>
#include <common.h>

void
psy_lbm_free_user(user_t* u){ 
  free(u->name);
  free(u->password);
  free(u);
}
