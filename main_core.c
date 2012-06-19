#include <stdio.h>
#include "main_core.h"
#include "medialib.h"
#include "play.h"
#include "db.h"
void *main_core(int cmd, void *data)
{
	static int state = 0;
	static char *now_play;
	static char *media_type;
	static int playlist_position;
	static int window_size[2];
	static link_t *mlink;
	static link_t *plink;
	int ctype;
	ctype = cmd / 1000;
	switch(ctype) {
		case 1:
			switch(cmd) {
				case GENERAL_MEDIALIB_INIT:
					mlink = (link_t *)data;
					break;
				case GENERAL_PLAYLIST_INIT:
					plink = (link_t *)data;
					break;
				case GENERAL_PLAY_MODE:
					state = *((int*)data);
					break;
				case GENERAL_VOLUME:
					general_volume(*((int*)data));
				case GENERAL_MUTE:
					general_mute(*((int*)data));
				default:
					return NULL;
			}
			break;
		case 2:
			switch(cmd) {
				case REQUEST_STATE:
					return &state;
					break;
				case REQUEST_NOW_PLAY:
					return &now_play;
					break;
				case REQUEST_MEDIA_TYPE:
					return media_type;
					break;
				case REQUEST_PLAYLIST_POSITION:
					return &playlist_position;
					break;
				case REQUEST_WINDOW_SIZE:
					return &window_size;
					break;
				case REQUEST_VOLUME:
					return request_volume();
					break;
				default:
					return NULL;
			}
			break;
		case 3:
			switch(cmd) {
				case MEDIALIB_SHOW:
					medialib_ui_show(mlink, data);
					break;
				case MEDIALIB_BRUSH:
					treeClass_data(mlink, data);
					treeMedialib_data(mlink, data);
					break;
				case MEDIALIB_ADD_FILE:
					medialib_add(mlink, data);
					break;
				case MEDIALIB_ADD_DIR:
					medialib_add_dir(mlink, data);
					break;
				case MEDIALIB_DEL_ALL:
					medialib_del_all(mlink, data);
					break;
				case MEDIALIB_SEARCH:
					medialib_search(mlink, data);
					break;
				case MEDIALIB_SEND_TO_PLAYLIST:
					medialib_sendto(plink, data);
					break;
				case MEDIALIB_FIND:
					medialib_find(mlink, data);
					break;
				case PLAYLIST_SHOW:
					playlist_ui_show(plink, data);
					break;
				case PLAYLIST_BRUSH:
					treePlaylist_data(plink, data);
					break;
				case PLAYLIST_ADD_FILE:
					playlist_add(plink, data);
					break;
				case PLAYLIST_ADD_DIR:
					playlist_add_dir(plink, data);
					break;
				case PLAYLIST_OPEN:
					playlist_open(plink, data);
					break;
				case PLAYLIST_SAVE:
					playlist_save(plink, data);
					break;
				case PLAYLIST_DEL:
					playlist_del(plink, data);
					break;
				case PLAYLIST_DEL_ALL:
					playlist_del_all(plink, data);
					break;
			}
			break;
		case 4:
			play(cmd, data);
			break;
		default:
			return NULL;
	}
	return NULL;
}

int general_sleep(void)
{
	return 0;
}
int general_play_mode(int *mode, InterFace *ui)
{
	return 0;
}
int general_volume(int volume)
{
	return 0;
}
int general_mute(int mute_switch)
{
	return 0;
}
void *request_volume(void)
{
	return 0;
}
