#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gst/gst.h>
#include <glib.h>
#include "play.h"
#include "main_core.h"
#include "medialib.h"
#include "interface.h"

//定义消息处理函数,
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
	GMainLoop *loop = (GMainLoop *) data;//这个是主循环的指针，在接受EOS消息时退出循环
	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_EOS:
		g_print("End of stream\n");
		g_main_loop_quit(loop);
		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);
		g_printerr("ERROR:%s\n", error->message);
		g_error_free(error);
		g_main_loop_quit(loop);
		break;
	}
	case GST_MESSAGE_TAG: {
		GstTagList *tags;
		gchar *title = "";
		gchar *artist = "";
		gst_message_parse_tag(msg, &tags);
		if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &title)
			 && gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist)) {
			puts(title); puts(artist);
		}
		gst_tag_list_free(tags);
		break;
	}
	default:
		break;
	}
	return TRUE;
}

int get_pos(GMainLoop *loop, int pos)
{
	return 0;
}

void play(int cmd, void *data)
{
	static char media_path[MAX_PATH_LENGTH];
	static int media_pos;
	static GMainLoop *loop;
	GstElement *pipeline;
	GstElement *source;
	GstElement *decoder;
	GstElement *sink;//定义组件
	GstBus *bus;
	switch(cmd) {
		case PLAY_STOP:
			g_main_loop_quit(loop);
			break;
		case PLAY_PAUSE:
			media_pos = get_pos(loop, 0);
			g_main_loop_quit(loop);
			break;
		case PLAY_PRE:
		case PLAY_NEXT:
		case PLAY_PITCH:
			media_path[0] = '\0';
			media_pos = 0;
			g_main_loop_quit(loop);
		default:
			if (media_path[0] != '\0')
				get_pos(loop, media_pos);
			else
				strcpy(media_path, (char *)data);
			gst_init(NULL, NULL);
			loop = g_main_loop_new(NULL, FALSE);
			pipeline = gst_pipeline_new("audio-player");
			source = gst_element_factory_make("filesrc", "file-source");
			decoder = gst_element_factory_make("mad", "mad-decoder");
			sink = gst_element_factory_make("autoaudiosink", "audio-output");
			if (!pipeline || !source || !decoder || !sink) {
				g_printerr("无法创建管道\n");
			}
			g_object_set(G_OBJECT(source), "location", media_path, NULL);
			bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
			gst_bus_add_watch(bus, bus_call, loop);
			gst_object_unref(bus);
			gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
			gst_element_link_many(source, decoder, sink, NULL);
			gst_element_set_state(pipeline, GST_STATE_PLAYING);

			g_main_loop_run(loop);
			gst_element_set_state(pipeline, GST_STATE_NULL);
			gst_object_unref(GST_OBJECT(pipeline));
	}
}
