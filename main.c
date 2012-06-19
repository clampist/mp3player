#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gtk-2.0/gtk/gtk.h>
#include <glib-2.0/glib.h>
#include <gst/gst.h>
#include <sqlite3.h>
#include <id3tag.h>
#include "main.h"
#include "main_core.h"
#include "medialib.h"
#include "db.h"
#include "interface.h"
#include "play.h"
#include "callback.h"

int main(int argc, char *argv[])
{
	if(open_database())
		create_medialib();
	else {
		g_print("错误：无法打开数据库。");
		return 1;
	}
	link_t mlink = {NULL, 0};
	link_t plink = {NULL, 0};
	load_medialib(&mlink, 0);
	
	main_core(GENERAL_MEDIALIB_INIT, &mlink);
	main_core(GENERAL_PLAYLIST_INIT, &plink);

	gtk_init(&argc, &argv);
	InterFace ui;
	ui.winMain = create_winMain(&ui);
	ui.diaMedialib = create_diaMedialib(&ui);
	ui.diaPlaylist = create_diaPlaylist(&ui);
	ui.diaVolume = create_diaVolume(&ui);
	gtk_widget_show(ui.winMain);
	g_signal_connect(G_OBJECT(ui.winMain), "delete_event",
			G_CALLBACK(gtk_main_quit), NULL);
	gtk_main();

	link_del_all(&mlink);
	link_del_all(&plink);
	close_database();
	return 0;
}
