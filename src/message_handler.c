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
    printf("username:%s\n", user);
    printf("password:%s\n", password);
  }

  else if (!strcmp(token, "ins")) {
    printf("received ins request\n");
  }

  else {
    printf("received weird command\n");
  }

}

