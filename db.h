#ifndef DB_H_
#define DB_H_
#include "medialib.h"
int open_database(void);
int close_database(void);
int create_medialib(void);
int load_medialib(link_t *mlink, find_cond order);
int medialib_insert(node_t *mnode);
int medialib_delete(node_t *mnode);
int medialib_delete_all(void);
#endif /* DB_H_ */
