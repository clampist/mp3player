#include "interface.h"
#ifndef MEDIALIB_H_
#define MEDIALIB_H_
#define MAX_TITLE_LENGTH 512
#define MAX_ATRIST_LENGTH 512
#define MAX_ALBUM_LENGTH 256
#define MAX_GENRE_LENGTH 256
#define MAX_PATH_LENGTH 4096
#define MAX_FILE_LENGTH 4096
typedef struct _medialib Medialib;
struct _medialib {
	char title[MAX_TITLE_LENGTH];
	char atrist[MAX_ATRIST_LENGTH];
	char album[MAX_ALBUM_LENGTH];
	char genre[MAX_GENRE_LENGTH];
	double record_time;
	char filepath[MAX_PATH_LENGTH];
};
typedef struct _playlist Playlist;
struct _playlist {
	char title[MAX_TITLE_LENGTH];
	double record_time;
	char filepath[MAX_PATH_LENGTH];
};
typedef struct _node_t node_t;
struct _node_t {
	Medialib item;
	node_t *p;
};
typedef struct _link_t link_t;
struct _link_t {
	node_t *np;
	int length;
};
typedef enum _find_cond find_cond;
enum _find_cond {
	BY_TITLE,
	BY_ARTIST,
	BY_ABLUM,
	BY_GENRE,
	BY_FILEPATH
};
int hazy_find(const char *str1, const char *str2);
int read_tag_from_file(const char *file,
		struct _medialib *media);
node_t *link_to_end(node_t *nt);
int link_add(link_t *mlink, const char *file);
int link_add_dir(link_t *mlink, const char *dir);
int link_find(link_t *mlink,
				  find_cond t,
		        const char *str2,
		        node_t **res);
int link_del(link_t *mlink, int p);
int link_del_all(link_t *mlink);
int link_search(link_t *mlink);
int is_mp3(const char *name);
int load_m3u(link_t *mlink, const char *file);
int save_m3u(link_t *mlink, const char *file);

void medialib_ui_show(link_t *mlink, InterFace *ui);
void treeClass_data(link_t *mlink, InterFace *ui);
void treeClass_init(link_t *mlink, InterFace *ui);
void treeMedialib_data(link_t *mlink, InterFace *ui);
void treeMedialib_init(link_t *mlink, InterFace *ui);
void medialib_add(link_t *mlink, const char *file);
void medialib_add_dir(link_t *mlink, const char *file);
void medialib_del(link_t *mlink, const char *file);
void medialib_del_all(link_t *mlink, InterFace *ui);
void medialib_search(link_t *mlink, InterFace *ui);
void medialib_sendto(link_t *plink, const char *file);
void medialib_find(link_t *mlink, InterFace *ui);

void playlist_ui_show(link_t *plink, InterFace *ui);
void treePlaylist_init(link_t *plink, InterFace *ui);
void treePlaylist_data(link_t *plink, InterFace *ui);
void playlist_add(link_t *plink, const char *file);
void playlist_add_dir(link_t *plink, const char *file);
void playlist_open(link_t *plink, const char *file);
void playlist_save(link_t *plink, const char *file);
void playlist_del(link_t *plink, InterFace *ui);
void playlist_del_all(link_t *plink, InterFace *ui);

#endif /* MEDIALIB_H_ */
