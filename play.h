#include <gst/gst.h>
#include <glib.h>

#ifndef PLAY_H_
#define PLAY_H_
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data);
void play(int cmd, void *data);

#endif /* PLAY_H_ */
