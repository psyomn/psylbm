#include <psylbm.h>
#include <domain.h>
#include <db_handler.h>
#include <message_handler.h>
#include <common.h>
#include <protocol_responses.h>

void
_psy_lbm_reply(psy_lbm_server_t* _s, remote_host_t* _h, char* _message) {
    sendto(_s->sock, 
           _message, strlen(_message), 
           0, (struct sockaddr*)&_h->a, _h->l);
}

void 
psy_lbm_handle_message(psy_lbm_server_t* _s, remote_host_t* _h, 
                       char* _message) {
  char delimiters[] = "|";
  char* token;
  int badrequest = 0;
  char* dupped_message = strdup(_message);

  /* First token */
  token = strtok(dupped_message, delimiters);

  if (token == NULL) {
    /* Someone sent something empty in the first token (eg: "|||") */
    token = "badrequest";
  }

  if (!strcmp(token, "auth")) {
    char* user = strtok(NULL, delimiters);
    char* pass = strtok(NULL, delimiters);
    psy_lbm_handle_authorization(_s, _h, user, pass);
  }

  else if (!strcmp(token, "ins")) {
    char *name    = strtok(NULL, delimiters),
         *title   = strtok(NULL, delimiters), 
         *volume  = strtok(NULL, delimiters), 
         *chapter = strtok(NULL, delimiters), 
         *page    = strtok(NULL, delimiters),
         *token   = strtok(NULL, delimiters);

    uint32_t i_vol  = atoi(volume),
             i_chap = atoi(chapter),
             i_page = atoi(page);

    printf("Received bookmark request [%s]-[%s]\n", title, token);
    psy_lbm_handle_insert(_s, _h, name, title, i_vol, i_chap, i_page, token);
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

  else {
    badrequest = 1;
  }

  if (badrequest) {
    /* TODO: a logger would be nice here */
    printf("Received malformed request [%s]\n", _message);
    _psy_lbm_reply(_s, _h, PSYLBM_BAD_REQUEST);
  }
  
  free(dupped_message);
}

/* provided a username and password generate a token, and send it back */
void
psy_lbm_handle_authorization(psy_lbm_server_t* _s, remote_host_t* _h,
                             char* _username, char* _password) {
  char* hash = NULL;
  int salt;
  user_t* u = psy_lbm_find_user_by_name(_s->db, _username);
  
  if (u == NULL) {
    /* No user found - send back fail message */
    _psy_lbm_reply(_s, _h, PSYLBM_AUTH_FAIL);
    return;
  }

  salt = atoi(u->salt);
  hash = _psy_lbm_hash_password(_password, salt);

  if (!strcmp(hash, u->password)) {
    char* token = _psy_lbm_generate_token();
    psy_lbm_handle_token(_s, _h, u->id, token);
    printf("Authenticate [%s]-[%s]\n", u->name, token);
    free(token);
  }
  else {
    /* TODO Log me */
    printf("Failed login attempt [%s]\n", u->name);
    _psy_lbm_reply(_s, _h, PSYLBM_AUTH_FAIL);
  }

  free(hash);
  psy_lbm_free_user(u);
}

/* Send the book info (title, vol, chapter, page), and if the token is a valid
 * login, then process the insert. The insert looks up if a same book has
 * previously been inserted. If that is the case, the info is updated. If not, a
 * new record is created.
 */
int
psy_lbm_handle_insert(psy_lbm_server_t* _s, remote_host_t* _h, 
                      char* _name, char* _title, uint32_t _vol, 
                      uint32_t _chapter, uint32_t _page, char* _token) {
  int ret;
  int32_t uid = psy_lbm_find_user_id_by_token(_s->db, _token);
  bookmark_t* bm = NULL;

  /* No such user */
  if (uid == -1) {
    _psy_lbm_reply(_s, _h, PSYLBM_BAD_REQUEST);
    return -1;
  }

  /* Does the previous bookmark exist? */
  bm = psy_lbm_find_bookmark_by_name(_s->db, _name);

  if (bm == NULL) {
    /* No such bookmark - inset */
    ret = psy_lbm_insert_bookmark(
      _s->db, uid, _name, _title, _vol, _chapter, _page);
  }
  else {
    /* Found bookmark - update */
    ret = psy_lbm_update_bookmark(
      _s->db, _name, _title, _vol, _chapter, _page);
  }

  if (!ret) 
    _psy_lbm_reply(_s, _h, PSYLBM_INS_OK);
  else 
    _psy_lbm_reply(_s, _h, PSYLBM_INS_FAIL);

  psy_lbm_free_bookmark(bm);

  return 0;
}

/*
 * Register an account. Return a status int.
 */
int
psy_lbm_handle_register(psy_lbm_server_t* _s, remote_host_t* _h, char* _user, 
                        char* _pass) {
  int ret;
  int usr_l = strlen(_user), pass_l = strlen(_pass); 
  int lengths_ok = 
    usr_l > 0 && usr_l <= PSYLBM_USERNAME_LENGTH &&
    pass_l > 0 && pass_l <= PSYLBM_PASSWORD_LENGTH;

  if (!lengths_ok) {
    _psy_lbm_reply(_s, _h, PSYLBM_BAD_REQUEST);
    return -1;
  }

  ret = psy_lbm_insert_user(_s->db, _user, _pass);

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

  return 0;
}

int
psy_lbm_handle_token(psy_lbm_server_t* _s, remote_host_t* _h, uint32_t _user_id,
                  char* _token) {
  int ret = psy_lbm_set_token(_s->db, _user_id, _token);

  if (!ret) { 
    psy_lbm_set_token(_s->db, _user_id, _token);

    /* Build response text */
    char resp[100];
    memset(resp, 0, sizeof(resp));
    strcpy(resp, PSYLBM_AUTH);
    strcat(resp, _token);
    _psy_lbm_reply(_s, _h, resp);
  } 
  else {
    _psy_lbm_reply(_s, _h, PSYLBM_SERVER_ERROR);
  }
  return 0;
}

/* TODO In case we ever want to log, this should handle the error */
int
psy_lbm_handle_error(char* _sent_stuff) {

  return 0;
}


