#pragma once
#include <pebble.h>

static void update_current_time_frame();
static void update_current_time_line();
static void update_todays_events();
static float get_current_time_position();
static void scroll_to_current_time();
//static void draw_event(Layer *layer, Event event, GContext *ctx);

//static void appmsg_in_dropped(AppMessageResult appmsg, DictionaryResult dicresult, void *context);
//static void dictionary_requst(DictionaryResult reason, void *context);
