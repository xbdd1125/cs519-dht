#include <stdio.h>

#include "client_api.h"

int main(int argc, char **argv) {
    int i;
    char buf[8192];

    char key_string[]   = "000_";
    char value_string[] = "abcdefg_";

    unsigned int len;
    char *data;
    int fd;

    if (argc < 3) {
        printf("Usage: client <addr> <port>\n");
        return -1;
    }

    fd = dht_connect(argv[1], argv[2]);
    
    for (i = 0; i < 10; i++) {
        
        key_string[3]   = '0' + i;
        value_string[7] = '0' + i;
        
        dht_write(fd, key_string, value_string);
    }
    
    key_string[3] = '0' + 5;
    dht_read(fd, key_string, buf);

    printf("Got: %s\n", buf);

    return 0;
}
