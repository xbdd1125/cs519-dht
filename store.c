#include <stdio.h>
#include <string.h>
#include <db.h>

#include "store.h"
#include "key.h"

static DB *dbp;

int init_store() {
    if (0 != db_create(&dbp, NULL, 0)) {
        printf("db create failed\n");
        return -1;
    }

    if (0 != dbp->open(dbp, NULL, NULL, NULL, DB_BTREE, DB_CREATE, 0)) {
        printf("db open failed\n");
        return -1;
    }

    return 0;
}

void close_store() {
    dbp->close(dbp, 0);
}


static void print_hex(char *str, int len) {
    int n;
    for(n=0; n < len; n++)
        printf("%02x", (unsigned char) str[n]);
}

void put_pair(char *key_hash, char *data, size_t data_len) {
    DBT key, value;

    memset(&key, 0, sizeof(DBT));
    memset(&value, 0, sizeof(DBT));

    key.data = key_hash;
    key.size = KEY_LENGTH;

    value.data = data;
    value.size = data_len;
    
    printf("Storing hash: ");
    print_hex(key_hash, KEY_LENGTH);
    printf("\n");

    dbp->put(dbp, NULL, &key, &value, 0);
}

void get_value(char *key_hash, char **data, size_t *data_len) {
    DBT key, value;

    memset(&key, 0, sizeof(DBT));
    memset(&value, 0, sizeof(DBT));

    key.data = key_hash;
    key.size = KEY_LENGTH;

    printf("Retrieving hash: ");
    print_hex(key_hash, KEY_LENGTH);
    printf("\n");

    dbp->get(dbp, NULL, &key, &value, 0);

    *data = value.data;
    *data_len = value.size;
}


