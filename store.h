#ifndef __STORE_H
#define __STORE_H

#define STORE_FILE "store.db"

int init_store();
void put_pair(char *, char *, size_t);
void get_value(char *, char **, size_t *);
void close_store();

#endif
