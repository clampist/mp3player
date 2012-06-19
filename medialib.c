#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <id3tag.h>
#include "medialib.h"
#include "interface.h"
#include "db.h"

int read_tag_from_file(const char *file,
		struct _medialib *media)
{
	struct id3_file *id3file;
	struct id3_tag *tag;
	struct id3_frame *frame_title;
	struct id3_frame *frame_artist;
	struct id3_frame *frame_album;
	struct id3_frame *frame_genre;
	struct id3_frame *frame_time;
	id3file = id3_file_open(file, ID3_FILE_MODE_READONLY);
	if (id3file == NULL) {
		printf("打开文件失败\n");
		return -1;
	}
	tag = id3_file_tag(id3file);
	frame_title = id3_tag_findframe(tag, ID3_FRAME_TITLE, 0);
	if (frame_title->fields)
		strcpy(media->title, (char*)frame_title->fields);
	frame_artist = id3_tag_findframe(tag, ID3_FRAME_ARTIST, 0);
	if (frame_artist->fields)
		strcpy(media->atrist, (char*)frame_artist->fields);
	frame_album = id3_tag_findframe(tag, ID3_FRAME_ALBUM, 0);
	if (frame_album->fields)
		strcpy(media->album, (char*)frame_album->fields);
	frame_genre = id3_tag_findframe(tag, ID3_FRAME_GENRE, 0);
	if (frame_genre->fields)
		strcpy(media->genre, (char*)frame_genre->fields);
	frame_time = id3_tag_findframe(tag, "TDRC", 0);
	if (frame_time->fields)
		media->record_time = (int)id3_field_getint(frame_time->fields);
	strcpy(media->filepath, file);
	id3_file_close(id3file);
	return 0;
}

node_t *link_to_end(node_t *nt)
{
	if (nt->p)
		return link_to_end(nt->p);
	else
		return nt;
}

int link_add(link_t *mlink, const char *file)
{
	node_t *endnode;
	node_t *mnode = (node_t *)malloc(sizeof(node_t));
	if (!mnode) {
		printf("分配内存失败\n");
		return 1;
	}
	if(read_tag_from_file(file, &mnode->item)) {
		printf("读取文件信息失败\n");
		free(mnode);
		return 1;
	}
	mnode->p = NULL;
	if (mlink->np) {
		endnode = link_to_end(mlink->np);
		endnode->p = mnode;
	}
	else
		mlink->np = mnode;
	mlink->length++;
	return 0;
}

int link_add_dir(link_t *mlink, const char *dir)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir(dir)) == NULL) {
		printf("无法打开该目录");
		return 0;
	}
	chdir(dir);
	while((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if(S_IFDIR & statbuf.st_mode) {
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
				continue;
			link_add_dir(mlink, entry->d_name);
		}
		else {
			if(is_mp3(entry->d_name))
				link_add(mlink, entry->d_name);
		}
	}
	chdir("..");
	closedir(dp);
	return 1;
}

int link_find(link_t *mlink,
				  find_cond t,
		        const char *str2,
		        node_t **res)
{
	if (*res) {
		free(*res);
		*res = NULL;
	}
	int i, l;
	int count = 0;
	const char *str1;
	node_t *tmp, **jump, **jump2;
	if (!mlink->length) {
		printf("媒体库为空，无法查找\n");
		return 0;
	}
	if (str2)
		l = strlen(str2);
	else {
		printf("查找字符串地址有误\n");
		return 0;
	}
	if (!l) {
		printf("查找字符串长度为0\n");
		return 0;
	}
	tmp = mlink->np;
	node_t *tmp_link = (node_t *)malloc(sizeof(node_t *) * mlink->length);
	jump = &tmp_link;
	while(tmp) {
		switch(t) {
			case BY_TITLE:
				str1 = tmp->item.title;
				break;
			case BY_ARTIST:
				str1 = tmp->item.atrist;
				break;
			case BY_ABLUM:
				str1 = tmp->item.album;
				break;
			case BY_GENRE:
				str1 = tmp->item.genre;
				break;
			case BY_FILEPATH:
				str1 = tmp->item.filepath;
				break;
			default:
				printf("查找条件出错");
				free(tmp_link);
				return 0;
		}
		if (str1 && strlen(str1)) {
			if (hazy_find(str1, str2)) {
				*(jump + count) = tmp;
				count++;
			}
		}
		tmp = tmp->p;
	}
	*res = (node_t *)malloc(sizeof(node_t *) * count);
	jump2 = res;
	for(i = 0; i < count; i++)
		*(jump2 + 0) = *(jump + 0);
	free(tmp_link);
	return count;
}

int link_del(link_t *mlink, int p)
{
	node_t *tmp, *tmp2;
	int i;
	if (p < 1 || p > mlink->length) {
		printf("输入的节点位置错误");
		return 1;
	}
	tmp = mlink->np;
	if (p == 1) {
		mlink->np = tmp->p;
		free(tmp);
	}
	else {
		i = p - 2;
		while(i) {
			tmp = tmp->p;
			i--;
		}
		tmp2 = tmp->p;
		tmp->p = tmp2->p;
		free(tmp2);
	}
	mlink->length--;
	return 0;
}

int link_del_all(link_t *mlink)
{
	node_t *tmp;
	if (mlink->length > 0) {
		do {
			tmp = mlink->np;
			mlink->np = tmp->p;
			free(tmp);
		} while(mlink->np);
		mlink->length = 0;
	}
	return 0;
}

int is_mp3(const char *name)
{
	int l;
	if (!name) {
		printf("文件名指针错误\n");
		return 0;
	}
	l = strlen(name);
	if (l < 5) {
		printf("输入的文件名有误\n");
		return 0;
	}
	l-=4;
	if (strcmp(name + l, ".mp3") && strcmp(name + l, ".MP3")) {
		return 0;
	}
	return 1;
}

int link_search(link_t *mlink) {
	return link_add_dir(mlink, "/");
}

int load_m3u(link_t *mlink, const char *file)
{
	char str[MAX_PATH_LENGTH];
	link_del_all(mlink);
	if (!file) {
		printf("文件链接错误\n");
		return 0;
	}
	FILE *fp = fopen(file, "r");
	if(fp != NULL) {
		fgets(str, MAX_PATH_LENGTH, fp);
		if(strncmp(str, "#EXTM3U", 7)) {
			printf("该文件不是播放列表文件\n");
			return 0;
		}
	}
	else {
		printf("错误代码：%d", ferror(fp));
		perror("打开文件错误\n");
	}
	while(!feof(fp)) {
		fgets(str, MAX_PATH_LENGTH, fp);
		str[strlen(str) - 1] = '\0';
		if (is_mp3(str))
			link_add(mlink, str);
	}
	fclose(fp);
	return 1;
}

int save_m3u(link_t *mlink, const char *file)
{
	FILE *fp;
	node_t *tmp;
	if (!file) {
		printf("文件链接错误\n");
		return 0;
	}
	if(creat(file, 0664) == -1) {
		perror("创建文件失败");
		return 0;
	}
	fp = fopen(file,"rw+");
	tmp = mlink->np;
	if (fp != NULL) {
		fprintf(fp, "#EXTM3U\n");
		while(tmp) {
			fprintf(fp, "#EXTINF:%d,%s\n%s\n\n",
					      (int) tmp->item.record_time,
					      tmp->item.title,
					      tmp->item.filepath);
			tmp = tmp->p;
		}
		fflush(fp);
	}
	else {
		perror("打开文件失败");
		return 0;
	}
	if (fclose(fp) != -1)
		printf("保存文件成功\n");
	else {
		perror("关闭文件");
		return 0;
	}
	return 1;
}

int hazy_find(const char *str1, const char *str2)
{
	const char *ct = "*?";
	int i, j = 0;	// str1 & str2's prt
	int l;	// length
	int k;		// one time jump
	char sec[MAX_FILE_LENGTH];
	const char *res2;
	int res;
	l = strlen(str2);
	if (l < 1)
		return 0;
	for (i = 0; i < l; i++) {
		if (str2[i] == '?') {
			j++;
		}
		else if (str2[i] == '*') {
			while(!(k = strcspn(str2 + i + 1, ct))) {
				if (*(str2 + i + 1))
					i++;
				else
					return 0;
			}
			strncpy(sec, str2 + i +1, k);
			sec[k] = '\0';
			res2 = strstr(str1 + j, sec);
			if (!res2)
				return 0;
			i += k;
			j += strlen(str1 + j) - strlen(res2) + k;
		}
		else {
			k = strcspn(str2 + i, ct);
			res = strncmp(str1 +j, str2 + i, k);
			if (res)
				return 0;
			else
				i += k - 1;
				j += k;
		}
	}
	return 1;
}

void medialib_ui_show(link_t *mlink, InterFace *ui)
{
	static int has_init = 0;
	if (!has_init) {
		treeClass_init(mlink, ui);
		treeClass_data(mlink, ui);
		treeMedialib_init(mlink, ui);
		treeMedialib_data(mlink, ui);
		has_init = 1;
	}
	gtk_widget_show(ui->diaMedialib);
}

void treeClass_init(link_t *mlink, InterFace *ui)
{

	GtkTreeStore *treestore;
	treestore = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	gtk_tree_view_set_model(GTK_TREE_VIEW(ui->treeClass),
	                       	GTK_TREE_MODEL(treestore));
	g_object_unref(G_OBJECT(treestore));
	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
	column = gtk_tree_view_column_new_with_attributes("分类", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ui->treeClass),
				column);
}

void treeClass_data(link_t *mlink, InterFace *ui)
{
	GtkTreeStore *treestore;
	treestore = (GtkTreeStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(ui->treeClass));
	gtk_tree_store_clear(treestore);
	const char *classtype[3] = {"艺术家", "专辑", "流派"};
	char *subclass, **subclass_sets, **subclass_tmp;
	int i, j, k, n;
	node_t *endnode;
	endnode = mlink->np;
	GtkTreeIter iter;
	GtkTreeIter child;
	GtkTreeIter grandson;
	gtk_tree_store_append(treestore, &iter, NULL);
	gtk_tree_store_set(treestore, &iter, 0, "全部", 1, -1, -1);
	for (i = 0; i < 3; i++) {
		j = 0;
		gtk_tree_store_append(treestore, &child, &iter);
		gtk_tree_store_set(treestore, &child, 0, classtype[i], 1, -1,-1);
		if (mlink->length) {
			while(endnode) {
				switch (i) {
					case 0:
						subclass = endnode->item.atrist;
						break;
					case 1:
						subclass = endnode->item.album;
						break;
					case 2:
						subclass = endnode->item.genre;
				}
				if (!j) {
					subclass_sets = (char **)malloc(sizeof(char **));
					subclass_sets[0] = subclass;
					j++;
					n = 0;
				}
				else {
					for(k = 0, n = 0; k < j; k++) {
						if (!strcmp(subclass_sets[k], subclass)) {
							n = 1;
							break;
						}
					}
					if (!n) {
						subclass_tmp = (char **)malloc(sizeof(char **) * (j + 1));
						for(k = 0; k < j; k++)
							subclass_tmp[k] = subclass_sets[k];
						subclass_tmp[j] = subclass;
						j++;
						free(subclass_sets);
						subclass_sets = subclass_tmp;
					}
				}
				if (!n) {
					gtk_tree_store_append(treestore, &grandson, &child);
					gtk_tree_store_set(treestore, &grandson, 0, subclass, 1, i, -1);
				}
				endnode = endnode->p;
			}
			free(subclass_sets);
			endnode = mlink->np;
		}
	}
}



void treeMedialib_data(link_t *mlink, InterFace *ui)
{
	node_t *endnode;
	endnode = mlink->np;
	GtkListStore *liststore;
	liststore = (GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(ui->treeMedialib));
	gtk_list_store_clear(liststore);
	GtkTreeIter iter;
	if (mlink->length) {
		while(endnode) {
			gtk_list_store_append(GTK_LIST_STORE(liststore), &iter);
			gtk_list_store_set(GTK_LIST_STORE(liststore), &iter,
						0, endnode->item.title,
						1, endnode->item.atrist,
						2, endnode->item.album,
						3, endnode->item.genre,
						4, (unsigned int) endnode->item.record_time,
						5, endnode->item.filepath,
						-1);
			endnode = endnode->p;
		}
	}
}

void treeMedialib_init(link_t *mlink, InterFace *ui)
{
	GtkListStore *liststore;
	liststore = gtk_list_store_new(6,
			                         G_TYPE_STRING,
                           		 G_TYPE_STRING,
			                         G_TYPE_STRING,
                           		 G_TYPE_STRING,
                           		 G_TYPE_UINT,
 			                         G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(ui->treeMedialib),
				              	GTK_TREE_MODEL(liststore));
	g_object_unref(G_OBJECT(liststore));
	GtkCellRenderer *renderer;
	//GtkTreeViewColumn *column;
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treeMedialib),
				           -1, "标题", renderer, "text", 0, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treeMedialib),
						   -1, "艺术家", renderer, "text", 1, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treeMedialib),
						   -1, "专辑", renderer, "text", 2, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treeMedialib),
						   -1, "流派", renderer, "text", 3, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treeMedialib),
						   -1, "记录时间", renderer, "text", 4, NULL);
}

void medialib_add(link_t *mlink, const char *file)
{
	node_t *endnode;
	endnode = mlink->np;
	link_add(mlink, file);
	endnode = link_to_end(endnode);
	medialib_insert(endnode);
}

void medialib_add_dir(link_t *mlink, const char *file)
{
	node_t *endnode;
	endnode = link_to_end(mlink->np);
	link_add_dir(mlink, file);
	endnode = endnode->p;
	while(!endnode) {
		medialib_insert(endnode);
		endnode = endnode->p;
	}
}

void medialib_del(link_t *mlink, const char *file)
{
	node_t *res = NULL;
	link_find(mlink, BY_FILEPATH, file, &res);
	medialib_delete(res);
	if(!res)
		free(res);
	load_medialib(mlink, 0);
}

void medialib_del_all(link_t *mlink, InterFace *ui)
{
	medialib_delete_all();
	load_medialib(mlink, 0);
}

void medialib_search(link_t *mlink, InterFace *ui)
{
	node_t *endnode;
	endnode = link_to_end(mlink->np);
	link_search(mlink);
	endnode = endnode->p;
	while(!endnode) {
		medialib_insert(endnode);
		endnode = endnode->p;
	}
}

void medialib_sendto(link_t *plink, const char *file)
{
	link_add(plink, file);
}

void medialib_find(link_t *mlink, InterFace *ui)
{
	node_t *res = NULL;
	find_cond fc;
	const char *str;
	int i, l;
	fc = gtk_combo_box_get_active (GTK_COMBO_BOX(ui->cbbFind));
	str = gtk_entry_get_text(GTK_ENTRY(ui->etFind));
	load_medialib(mlink, 0);
	l = link_find(mlink, fc, str, &res);
	link_del_all(mlink);
	for(i = 0; i < l; i++)
		link_add(mlink, (res + i)->item.filepath);
	if (res)
		free(res);
}

void treePlaylist_init(link_t *mlink, InterFace *ui)
{
	GtkListStore *liststore;
	liststore = gtk_list_store_new(6,
			                         G_TYPE_STRING,
                           		 G_TYPE_STRING,
			                         G_TYPE_STRING,
                           		 G_TYPE_STRING,
                           		 G_TYPE_UINT,
 			                         G_TYPE_STRING);

	gtk_tree_view_set_model(GTK_TREE_VIEW(ui->treePlaylist),
				              	GTK_TREE_MODEL(liststore));
	g_object_unref(G_OBJECT(liststore));
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treePlaylist),
				           -1, "标题", renderer, "text", 0, NULL);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(ui->treePlaylist),
						   -1, "记录时间", renderer, "text", 4, NULL);
}

void treePlaylist_data(link_t *plink, InterFace *ui)
{
	node_t *endnode;
	endnode = plink->np;
	GtkListStore *liststore;
	liststore = (GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(ui->treePlaylist));
	gtk_list_store_clear(liststore);
	GtkTreeIter iter;
	if (plink->length) {
		while(endnode) {
			gtk_list_store_append(GTK_LIST_STORE(liststore), &iter);
			gtk_list_store_set(GTK_LIST_STORE(liststore), &iter,
						0, endnode->item.title,
						1, endnode->item.atrist,
						2, endnode->item.album,
						3, endnode->item.genre,
						4, (unsigned int) endnode->item.record_time,
						5, endnode->item.filepath,
						-1);
			endnode = endnode->p;
		}
	}
}

void playlist_ui_show(link_t *plink, InterFace *ui)
{
	static int has_init = 0;
	if (!has_init) {
		treePlaylist_init(plink, ui);
		treePlaylist_data(plink, ui);
		has_init = 1;
	}
	gtk_widget_show(ui->diaPlaylist);
}

void playlist_add(link_t *plink, const char *file)
{
	link_add(plink, file);
}

void playlist_add_dir(link_t *plink, const char *file)
{
	link_add_dir(plink, file);
}

void playlist_open(link_t *plink, const char *file)
{
	load_m3u(plink, file);
}

void playlist_save(link_t *plink, const char *file)
{
	save_m3u(plink, file);
}

void playlist_del(link_t *plink, InterFace *ui)
{
	int id;
	GtkListStore *liststore;
	GtkTreeSelection *selection;
	liststore = (GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(ui->treePlaylist));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ui->treePlaylist));
	id = gtk_tree_selection_count_selected_rows (selection);
	link_del(plink, id);
}

void playlist_del_all(link_t *plink, InterFace *ui)
{
	link_del_all(plink);
}
