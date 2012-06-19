#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "db.h"
#include "medialib.h"
sqlite3 *db = NULL;

int open_database(void)
{
	int res;
	const char *filename = "data.sqlite";
	res = sqlite3_open_v2(filename,
							    &db,
							    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
							    NULL);
	if (res) {
		printf("%s\n", sqlite3_errmsg(db));
		return 0;
	}
	return 1;
}

int close_database(void)
{
	int res;
	res = sqlite3_close(db);
	if (res) {
		printf("%s\n", sqlite3_errmsg(db));
		return 0;
	}
	return 1;
}

int create_medialib(void)
{
   int res;
   char *errmsg = NULL;
   const char *sqlcmd = "CREATE TABLE 'medialib' ('title' TEXT,\
		                                            'artist' TEXT,\
		                                            'album' TEXT,\
		                                            'genre' TEXT,\
		                                            'record_time' NUMERIC,\
		                                            'filepath' TEXT)";
   res = sqlite3_exec( db, sqlcmd, 0, 0, &errmsg );
   if (res) {
	   printf("创建数据表失败：%d-%s\n", res, errmsg );
	   return 0;
   }
   return 1;
}

int load_medialib(link_t *mlink, find_cond t)
{
	int res;
	char *errmsg = NULL;
	char **tb;
	int i;
	int nrow;
	int ncol;
	char sqlcmd[256];
	const char *str = NULL;
	node_t *endnode = NULL;
	node_t *mnode = NULL;
	link_del_all(mlink);
	switch(t) {
		case BY_TITLE:
			str = "title";
			break;
		case BY_ARTIST:
			str = "artist";
			break;
		case BY_ABLUM:
			str = "album";
			break;
		case BY_GENRE:
			str = "genre";
			break;
		case BY_FILEPATH:
			str = "filepath";
			break;
	}
	if (str) {
		sprintf(sqlcmd, "SELECT * FROM medialib ORDER BY %s", str);
	}
	else {
		sprintf(sqlcmd, "SELECT * FROM medialib");
	}
	res = sqlite3_get_table(db,sqlcmd,&tb,&nrow,&ncol,&errmsg);
	if (res) {
		printf("读取数据库失败：%d-%s\n", res, errmsg );
		return 0;
	}
	else {
		if (nrow > 0) {
			for(i = 0; i < nrow; i++) {
				mnode = (node_t *)malloc(sizeof(node_t));
				strcpy(mnode->item.title, tb[(i + 1) * ncol + 0]);
				strcpy(mnode->item.atrist, tb[(i + 1) * ncol + 1]);
				strcpy(mnode->item.album, tb[(i + 1) * ncol + 2]);
				strcpy(mnode->item.genre, tb[(i + 1) * ncol + 3]);
				mnode->item.record_time = atoi(tb[(i + 1) * ncol + 4]);
				strcpy(mnode->item.filepath, tb[(i + 1) * ncol + 5]);
				if (i) {
					endnode->p = mnode;
					mnode->p = NULL;
					endnode = mnode;
					mlink->length++;
				}
				else {
					endnode = mnode;
					mlink->np = mnode;
					mlink->length = 1;
				}
			}
		}
	}
	return 1;
}

int medialib_insert(node_t *mnode)
{
	int res;
	char *errmsg = NULL;
	char **tb;
	int nrow;
	int ncol;
	char sqlcmd[MAX_TITLE_LENGTH
	          + MAX_ATRIST_LENGTH
	          + MAX_ALBUM_LENGTH
	          + MAX_GENRE_LENGTH
	          + MAX_PATH_LENGTH
	              + 256];
	if (!mnode) {
		printf("链表节点地址有误\n");
		return 0;
	}
	sprintf(sqlcmd,
			  "SELECT title FROM medialib WHERE filepath = \"%s\"",
			  mnode->item.filepath);
	res = sqlite3_get_table(db,sqlcmd,&tb,&nrow,&ncol,&errmsg);
	if (res) {
		printf("读取数据库失败：%d-%s\n", res, errmsg );
		return 0;
	}
	if (nrow > 0) {
		sprintf(sqlcmd,
			     "UPDATE medialib SET title = \"%s\",\
			                          artist = \"%s\",\
			                          album = \"%s\",\
			                          genre = \"%s\",\
			                          record_time = %d \
			      WHERE filepath = \"%s\"",
			      mnode->item.title,
			      mnode->item.atrist,
			      mnode->item.album,
			      mnode->item.genre,
			      (int) mnode->item.record_time,
			      mnode->item.filepath);
	}
	else {
		sprintf(sqlcmd,
			     "INSERT INTO medialib VALUES \
			     (\"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\")",
			     mnode->item.title,
			     mnode->item.atrist,
			     mnode->item.album,
			     mnode->item.genre,
			     (int) mnode->item.record_time,
			     mnode->item.filepath);
	}
	res = sqlite3_exec(db, sqlcmd, 0, 0, &errmsg);
	if (res) {
		printf("写入数据库失败：%d-%s\n", res, errmsg );
		return 0;
	}
	return 1;
}

int medialib_delete(node_t *mnode)
{
	int res;
	char *errmsg = NULL;
	char sqlcmd[MAX_PATH_LENGTH + 256];
	if (!mnode) {
		printf("链表节点地址有误\n");
		return 0;
	}
	sprintf(sqlcmd,
		     "DELETE FROM medialib WHERE filepath = \"%s\"",
		     mnode->item.filepath);
	res = sqlite3_exec( db, sqlcmd, 0, 0, &errmsg );
	if (res) {
		printf("删除记录失败：%d-%s\n", res, errmsg );
		return 0;
	}
	return 1;
}

int medialib_delete_all(void)
{
	int res;
	char *errmsg = NULL;
	char sqlcmd[256];
	sprintf(sqlcmd, "DELETE FROM medialib");
	res = sqlite3_exec( db, sqlcmd, 0, 0, &errmsg );
	if (res) {
		printf("删除记录失败：%d-%s\n", res, errmsg );
		return 0;
	}
	return 1;
}
