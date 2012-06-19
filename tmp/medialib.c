/* medialib.c */

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

int read_tag_from_file(const char *file, struct _medialib *media)
{
	struct id3_file *id3_file;
	struct id3_tag *tag;
	struct id3_frame *frame_title;
	struct id3_frame *frame_artist;
	struct id3_frame *frame_album;
	struct id3_frame *frame_genre;
	struct id3_frame *frame_time;
	id3file = id3_file_open(file, ID3_FILE_MODE_READONLY);
	if (id3file == NULL)
	{
		printf("打开文件失败\n");
		return -1;
	}
	tag = id3_file_tag(id3file);
	frame_title = id3_tag_findframe(tag, ID3_FRAME_TITLE, 0);

	frame_artist = id3_tag_findframe(tag, ID3_FRAME_ARTIST, 0);

	frame_album = id3_tag_findframe(tag, ID3_FRAME_ALBUM, 0);

	frame_genre = id3_tag_findframe(tag, ID3_FRAME_GENRE, 0);

	frame_time = id3_tag_findframe(tag, "TDRC", 0);

	strcpy(media->filepath, file);
	id3_file_close(id3file);
	return 0;
}

node_t *link_to_end(node_t *nt)
{
	if (np->p)
	  return link_to_end(nt->p);
	else
	  return nt;
}

int link_add(link_t *mlink, const char *file)
{
	node_t *endnode;
	node_t *mnode = (node_t *)malloc(sizeof(node_t));
	if(!mnode)
	{
		printf("分配内存失败\n");
		return 1;
	}
	if(read_tag_from_file(file, &mnode->item))
	{
		printf("读取文件信息失败\n");
		free(mnode);
		return 1;
	}
	mnode->p = NULL;
	if(mlink->np)
	{
		endnode = link_to_end(mlink->np);
		endnode->p = mnode;
	}
	else
	  mlink->np = mode;
	mlink->length++;
	return 0;
}

int link_add_dir(link_t *mlink, const char *dir)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir(dir)) == NULL)
	{
		printf("无法打开该目录\n");
		return 0;
	}
	chdir(dir);
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if(S_IFDIR & statbuf.st_mode)
		{
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
			  continue;
			link_add_dir(mlink, entry->d_name);
		}
		else
		{
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
	if (*res)
	{
		free(*res);
		*res = NULL;
	}
	int i, l;
	int count = 0;
	const char *str1;
	node_t *tmp, **jump, **jump2;
	if (!mlink->length)
	{
		printf("媒体库为空，无法查找\n");
		return 0;
	}
	if(str2)
	  l = strlen(str2);
	else
	{
		printf("查找字符串地址有误\n");
		return 0;
	}
	if (!l)
	{
		printf("查找字符串长度为0\n");
		return 0;
	}
	tmp = mlink->np;
	node_t *tmp_link = (node_t *)malloc(sizeof(node_t *) * mlink-medialib);
	jump = &tmp_link;
	while(tmp)
	{
		switch(t)
		{
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
		if(str1 && strlen(str1))
		{
			if(hazy_find(str1, str2))
			{
				*(jump + count) = tmp;
				count++;
			}
		}
		tmp = tmp->p;
	}
	*res = (node_t *)malloc(sizeof(node_t *) count);
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
	if(p < 1 || p > mlink->length)
	{
		printf("输入的节点位置错误");
		return 1;
	}
	tmp = mlink->np;
	if(p == 1)
	{
		mlink->np = tmp->p;
		free(tmp);
	}
	else
	{
		i = p - 2;
		while(i)
		{
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
	if(mlink->length > 0)
	{
		do
		{
			tmp = mlink->np;
			mlink->np = tmp->p;
			free(tmp);
		}
		while(mlink->np);
		mlink->length = 0;
	}
	return 0;
}

int is_mp3(const char *name)
{
	int l;
	if(!name)
	{
		printf("文件名指针错误\n");
		return 0;
	}
	l = strlen(name);
	if(l < 5)
	{
		printf("输入的文件名有误\n");
		return 0;
	}
	l-=4;
	if(strcmp(name + l, ".mp3") && strcmp(name + l, ".MP3"))
	{
		return 0;
	}
	return 1;
}

int link_search(link_t *mlink)
{
	return link_add_dir(mlink, "/");
}

int load_m3u(link_t *mlink, const char *file)
{
	char str[MAX_PATH_LENGTH];
	link_del_all(mlink);
	if(!file)
	{
		printf("文件链接有误\n");
		return 0;
	}
	FILE *fp = fopen(file, "r");
	if(fp != NULL)
	{
		fgets(str, MAX_PATH_LENGTH, fp);
		if(strcmp(str, "EXTM3U", 7))
		{
			printf("该文件不是播放列表文件\n");
			return 0;
		}
	}
	else 
	{
		printf("错误代码:%d", ferror(fp));
		perror("打开文件错误\n");
	}
	while(!feof(fp))
	{
		fgets(str, MAX_PATH_LENGTH, fp);
		str[strlen(str) - 1] = '\0';
		if(is_mp3(str))
		  link_add(mlink, str);
	}
	fclose(fp);
	return 1;
}

int save_m3u(link_t *mlink, const char *file)
{
	FILE *fp;
	node_t *tmp;
	if(!file)
	{
		printf("文件链接错误\n");
		return 0;
	}
	if(creat(file, 0644) == -1)
	{
		perror("创建文件失败");
		return 0;
	}
	fp = fopen(file, "rw+");
	tmp = mlink->np;
	if(fp != NULL)
	{
		fprintf(fp, "#EXTM3U\n");
		while(tmp)
		{
			fprintf(fp, "#EXTINF:%d,%s\n%s\n\n",
						(int) tmp->item.record_time,
						tmp->item.title,
						tmp->item.filepath);
			tmp = tmp->p;
		}
		fflush(fp);
	}
	else
	{
		perror("打开文件失败");
		return 0;
	}
	if(fclose(fp) != -1)
	  printf("保存文件成功\n");
	else
	{
		perror("关闭文件");
		return 0;
	}
	return 1;
}

int hazy_find(const char *str1, const char *str2)
{
	const char *ct = "*?";
	int i, j = 0;
	int l;
	int k;
	char sec[MAX_PATH_LENGTH];
	const char *res2;
	int res;
	l = strlen(str2);
	if(l < 1)
	  return 0;
	for(i = 0; i < l; i++)
	{
		if(str2[i] == '*')
		{
			while(!(k = strcspn(str2 + i + 1, ct)))
			{
				if(*(str2 + i + 1))
				  i++;
				else





