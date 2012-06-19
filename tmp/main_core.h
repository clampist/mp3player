/* main_core.h */

#include "interface.h"

#ifndef MAIN_CORE_H_
#define MAIN_CORE_H_

#define GENERAL_MEDIALIB_INIT 1000
#define GENERAL_PLAYLIST_INIT 1001
#define GENERAL_PLAY_MODE 1002
#define GENERAL_VOLUME 1003
#define GENERAL_MUTE 1004

#define REQUEST_STATE 2000
#define REQUEST_NOW_PLAY 2001
#define REQUEST_MEDIA_TYPE 2002
#define REQUEST_PLAYLIST_POSITION 2003
#define REQUEST_WINDOWS_SIZE 2004
#define REQUEST_VOLUME 2005

#define MEDIALIB_SHOW 3000
#define MEDIALIB_BRUSH 3001
#define MEDIALIB_ADD_FILE 3002
#define MEDIALIB_ADD_FILE 3003
#define MEDIALIB_FIND 3004
#define MEDIALIB_SEARCH 3005
#define MEDIALIB_DEL 3006
#define MEDIALIB_DEL_ALL 3007
#define MEDIALIB_SELECT_CLASS 3008
#define MEDIALIB_SELECT_FILE 3009
#define MEDIALIB_SEND_TO_PLAYLIST 3010

#define PLAYLIST_SHOW 3100
#define PLAYLIST_BRUSH 3101
#define PLAYLIST_ADD_FILE 3102
#define PLAYLIST_ADD_DIR 3103
#define PLAYLIST_DEL 3104
#define PLAYLIST_DEL_ALL 3105
#define PLAYLIST_SELECT 3106
#define PLAYLIST_COMPOSITOR_BY_NAME 3107
#define PLAYLIST_OPEN 3108
#define PLAYLIST_SAVE 3109
#define PLAYLIST_COMP 3110

#define PLAY_START 4000
#define PLAY_STOP 4001
#define PLAY_PAUSE 4002
#define PLAY_PRE 4003
#define PLAY_NEXT 4004
#define PLAY_PITCH 4005

int general_sleep(void);
int general_play_mode(int *mode, InterFace *ui);
int general_volume(int volume);
int general_mute(int mute_switch);
void *request_volume(void);
void *main_core(int cmd, void *date);

#endif 

/* MAIN_CORE_H */
