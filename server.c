#include <stdio.h>
#include <string.h>

#include "config.h"
#include "key.h"
#include "store.h"

int main(int argc, char **argv) {

    struct config *cfg;
    char hash[20];
    char *data;
    size_t len;

    if (argc < 2) {
        printf("Usage: server <num>\n");
        return -1;
    }

    get_config("dht.cfg", &cfg);

    if ((cfg->num_servers % 2) == 1) {
        printf("There must be an ever number of servers!\n");
        goto err;
    }
    printf("Total servers: %u\n", cfg->num_servers);

    cfg->self_entry = atoi(argv[1]) - 1;
    if (cfg->self_entry > (cfg->num_servers - 1)) {
        printf("Invalid entry number!\n");
        goto err;
    }

    printf("Using entry %u from dht.cfg\n", cfg->self_entry);
    printf("Local address: %s:%s (%s)\n", cfg->servers[cfg->self_entry].address,
                                          cfg->servers[cfg->self_entry].port,
                                          cfg->servers[cfg->self_entry].is_primary ? "primary" : "secondary");

    if (0 != init_store()) {
        printf("Failed to initialize store\n");
        goto err;
    }

    free_config(cfg);
    close_store();

    return 0;

err:
    free_config(cfg);
    return -1;
}
