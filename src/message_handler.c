#include <domain.h>
#include <db_handler.h>
#include <message_handler.h>
#include <common.h>
#include <protocol_responses.h>

void 
psy_lbm_handle_message(psy_lbm_server_t* _s, remote_host_t* _h, 
                       char* _message) {
  char delimiters[] = "|";
  char *token;
  int badrequest = 0;

  /* First token */
  token = strtok(_message, delimiters);

  if (token == NULL) {
    /* Someone sent something empty in the first token (eg: "|||") */
    token = "badrequest";
  }

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
    char* user = strtok(NULL, delimiters);
    char* pass = strtok(NULL, delimiters);
    printf("Registration request [%s]\n", user);
    
    if (user != NULL && pass != NULL)
      psy_lbm_handle_register(_s, _h, user, pass);
    else {
      badrequest = 1;
    }
  }

  if (badrequest) {
    /* TODO: a logger would be nice here */
    printf("Received bad request [%s]\n", _message);
    sendto(_s->sock, PSYLBM_BAD_REQUEST, sizeof(PSYLBM_BAD_REQUEST), 
           0, (struct sockaddr*)&_h->a, _h->l);
  }

}

/*
 * provided a username and password generate a token, and send it back
 */
char*
psy_lbm_handle_authorization(psy_lbm_server_t* _s, remote_host_t* _h,
                             char* _username, char* _password) {

}

/* Send the book info (title, vol, chapter, page), and if the token is a valid
 * login, then process the insert. The insert looks up if a same book has
 * previously been inserted. If that is the case, the info is updated. If not, a
 * new record is created.
 */
int
psy_lbm_handle_insert(psy_lbm_server_t* _s, remote_host_t* _h, char* _title, 
                      uint32_t _vol, uint32_t _chapter, uint32_t _page, 
                      char* _token) {

}

/*
 * Register an account. Return a status int.
 */
int
psy_lbm_handle_register(psy_lbm_server_t* _s, remote_host_t* _h, char* _user, 
                        char* _pass) {
  int ret = psy_lbm_insert_user(_s->db, _user, _pass);

  if (ret == -1) {
    /* Some problem with query */
    sendto(_s->sock, PSYLBM_SERVER_ERROR, sizeof(PSYLBM_SERVER_ERROR), 
           0, (struct sockaddr*)&_h->a, _h->l);
  }
  else if (ret == -2) {
    /* User already exist */
    sendto(_s->sock, PSYLBM_USERNAME_TAKEN, sizeof(PSYLBM_USERNAME_TAKEN), 
           0, (struct sockaddr*)&_h->a, _h->l);
    printf("--| Username [%s] has been taken.\n", _user); /* TODO: logme */
  }
  else {
    /* Everything ok */
    sendto(_s->sock, PSYLBM_REGISTRATION_OK, sizeof(PSYLBM_REGISTRATION_OK), 
           0, (struct sockaddr*)&_h->a, _h->l);
    printf("--| Username [%s] registered.\n", _user); /* TODO: logme */
  }
}

/* TODO In case we ever want to log, this should handle the error */
int
psy_lbm_handle_error(char* _sent_stuff) {

}
