#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "config.h"

void free_config(struct config *complete_config) {
    int i;

    for (i = 0; i < complete_config->num_servers; i++) {
        free(complete_config->servers[i].address);
        free(complete_config->servers[i].port);
    }

    free(complete_config->servers);
}


static char *copy_token(char *token) {
    char *copy = (char *) malloc(strlen(token));
    strcpy(copy, token);
    return copy;
}

int get_config(char *file, struct config **complete_config) {
    size_t buffer_len = 80;
    char *line_buffer;
    struct config *server_config;
    FILE *fh;
    int return_value = 0;
    unsigned int line_number = 0;

    line_buffer = (char *) malloc(buffer_len);
    if (0 == line_buffer) 
        return -1;

    fh = fopen(file, "r");
    if (0 == fh) {
        return_value = -1;
        goto ret;
    }
    
    server_config = (struct config *) malloc(sizeof(struct config));
    if (0 == server_config) {
        return_value = -1;
        goto ret;
    }
    

    while ( getline(&line_buffer, &buffer_len, fh) > 0 ) {
        char *tokptr;

        line_number += 1;

        if (1 == line_number) { /* First line is how many servers total */
            server_config->num_servers = atoi(line_buffer);
            server_config->servers     = (struct server_info *) 
                                         malloc(server_config->num_servers *
                                                sizeof(struct server_info));
            continue;
        }
        
        /* Address entry */
        tokptr = (char *) strtok(line_buffer, ":");
        server_config->servers[line_number - 2].address = copy_token(tokptr);

        /* Port number entry */
        tokptr = strtok(NULL, ":");
        server_config->servers[line_number - 2].port = copy_token(tokptr);
        
        /* Primary/Secondary indicator entry */
        tokptr = strtok(NULL, ":");
        server_config->servers[line_number - 2].is_primary = tokptr[0] == 'p' ? 1 : 0;
    }

    *complete_config = server_config;

ret:
    fclose(fh);
    free(line_buffer);
    return return_value;
}
