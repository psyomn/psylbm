#pragma once

// TODO: get rid of this file

#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include <openssl/sha.h>

// TODO: this is here because sqlite3 returns (rightfully) const
// unsigned char* strings from the database, because UTF-8. I should
// revisit the structures handling strings, and fix them. That will
// have to be yet another maintenance pass.
char *psylbm_strndup(const unsigned char *src, size_t len);
