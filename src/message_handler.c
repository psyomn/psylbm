#include <domain.h>
#include <db_handler.h>
#include <message_handler.h>
#include <common.h>
#include <protocol_responses.h>

static void
_psy_lbm_reply(psy_lbm_server_t* _s, remote_host_t* _h, char* _message) {
    sendto(_s->sock, _message, strlen(_message), 
           0, (struct sockaddr*)&_h->a, _h->l);
}

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
    char* pass = strtok(NULL, delimiters);
    printf("auth [%s]\n", user);
    psy_lbm_handle_authorization(_s, _h, user, pass);
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
  char* ret  = NULL;
  char* hash = NULL;
  int salt;
  user_t* u = psy_lbm_find_user_by_name(_s->db, _username);
  
  if (u == NULL) {
    /* No user found - send back fail message */
    sendto(_s->sock, PSYLBM_AUTH_FAIL, sizeof(PSYLBM_AUTH_FAIL), 
           0, (struct sockaddr*)&_h->a, _h->l);
    return ret;
  }

  printf("found: \n"
    "  name: [%s]\n"
    "  hash: [%s]\n"
    "  salt: [%s]\n", u->name, u->password, u->salt);

  salt = atoi(u->salt);
  hash = _psy_lbm_hash_password(_password, salt);

  printf("Calculated hash: %s\n", hash);

  if (!strcmp(hash, u->password)) {
    printf("login possible\n");
    char* token = _psy_lbm_generate_token();
    printf("  generated token: [%s]\n", token);
    psy_lbm_handle_token(_s, _h, u->id, token);
    free(token);
  }
  else {
    printf("passwords don't match\n");
  }

  free(hash);
  psy_lbm_free_user(u);

  return ret;
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
    _psy_lbm_reply(_s, _h, PSYLBM_SERVER_ERROR);
  }
  else if (ret == -2) {
    /* User already exist */
    _psy_lbm_reply(_s, _h, PSYLBM_USERNAME_TAKEN);
    printf("--| Username [%s] has been taken.\n", _user); /* TODO: logme */
  }
  else {
    /* Everything ok */
    _psy_lbm_reply(_s, _h, PSYLBM_REGISTRATION_OK);
    printf("--| Username [%s] registered.\n", _user); /* TODO: logme */
  }
}

int
psy_lbm_handle_token(psy_lbm_server_t* _s, remote_host_t* _h, uint32_t _user_id,
                  char* _token) {
  int ret = psy_lbm_set_token(_s->db, _user_id, _token);

  if (!ret) { 
    psy_lbm_set_token(_s->db, _user_id, _token);
    _psy_lbm_reply(_s, _h, _token);
  } 
  else {
    _psy_lbm_reply(_s, _h, "weird problems arise");
  }
  return 0;
}

/* TODO In case we ever want to log, this should handle the error */
int
psy_lbm_handle_error(char* _sent_stuff) {

}
