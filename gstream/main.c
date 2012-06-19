/* main.c */
#include <gst/gst.h>
#include <stdbool.h>
#include <stdio.h>

static GMainLoop *loop;

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer
