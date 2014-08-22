#include "message_handler.h"
#include "common.h"

void 
psy_lbm_handle_message(char* _message) {
  char delimiters[] = "|";
  char *token;

  /* First token */
  token = strtok(_message, delimiters);

  printf("%s\n",token);

  if (!strcmp(token, "auth")) {
    printf("received auth request\n");
    char* user = strtok(NULL, delimiters);
    char* password = strtok(NULL, delimiters);
    printf("auth [%s]\n", user);
  }

  else if (!strcmp(token, "ins")) {
    printf("received ins request\n");
  }

  else if (!strcmp(token, "reg")) {
    printf("Registration request\n");
    char* user = strtok(NULL, delimiters);
    char* pass = strtok(NULL, delimiters);
    printf("  user: %s\n", user);

    psy_lbm_handle_register(user, pass);
  }

  else {
    printf("received weird command\n");
  }

}

/*
 * provided a username and password generate a token, and send it back
 */
char*
psy_lbm_handle_authorization(char* _username, char* _password) {

}

/* Send the book info (title, vol, chapter, page), and if the token is a valid
 * login, then process the insert. The insert looks up if a same book has
 * previously been inserted. If that is the case, the info is updated. If not, a
 * new record is created.
 */
int
psy_lbm_handle_insert(char* _title, uint32_t _vol ,uint32_t _chapter, 
                      uint32_t _page, char* _token) {

}

/*
 * Register an account. Return a status int.
 */
int
psy_lbm_handle_register(char* _user, char* _pass) {
  /* Check if the account exists */

  return 1;
}

/* In case we ever want to log, this should handle the error */
int
psy_lbm_handle_error(char* _sent_stuff) {

}
