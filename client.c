#include <stdio.h>

#include "client_api.h"

static void run_test(char *addr, char *port) {
    int i;
    char buf[8192];

    char key_string[]   = "000_";
    char value_string[] = "abcdefg_";

    unsigned int len;
    char *data;
    int fd;

    fd = dht_connect(addr, port);
    
    for (i = 0; i < 20; i++) {
        
        key_string[3]   = '0' + i;
        value_string[7] = '0' + i;
        
        dht_write(fd, key_string, value_string);
    }
    
    key_string[3] = '0' + 5;
    dht_read(fd, key_string, buf);

    printf("Got: %s... ", buf);
    if (0 == strcmp (buf, "abcdefg5"))
        printf("OK\n");
    else
        printf("FAILED\n");

    dht_kill(fd);
    dht_disconnect(fd);

}

int main(int argc, char **argv) {
    char buf[8192];
    int fd;

    if (argc < 2) {
        printf("Usage: client <cmd> <args...>\n");
        return -1;
    }

    switch (argv[1][0]) {
        
        case ('t'):
            if (argc < 4) {
                printf("Not enough arguments for the test\n");
                return -1;
            }

            run_test(argv[2], argv[3]);
            break;

        case ('r'):
            if (argc < 5) {
                printf("Not enough arguments for a read\n");
                return -1;
            }
            
            fd = dht_connect(argv[2], argv[3]);
            dht_read(fd, argv[4], buf);
            printf("\nReceived: %s\n", buf);
            dht_disconnect(fd);
            break;

        case ('w'):
            if (argc < 6) {
                printf("Not enough arguments for a write\n");
                return -1;
            }
            
            fd = dht_connect(argv[2], argv[3]);
            dht_write(fd, argv[4], argv[5]);
            dht_disconnect(fd);

        case ('k'):
            if (argc < 4) {
                printf("Not enough arguments for kill\n");
                return -1;
            }

            fd = dht_connect(argv[2], argv[3]);
            dht_kill(fd);
            printf("\nKilled DHT\n");
            dht_disconnect(fd);

            break;

        default:
            break;
    }

    return 0;
}

