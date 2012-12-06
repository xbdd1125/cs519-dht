#include <openssl/sha.h>
#include <stdlib.h>

void get_key(char *buf, size_t len, char *keyval) {
    SHA1(buf, len, keyval);
}

unsigned int who_has_key(char *keyval, unsigned int num_servers) {
   unsigned short first = ((unsigned short *) keyval)[0];

   return first / ((256 * 256) / (num_servers >> 2));
}
