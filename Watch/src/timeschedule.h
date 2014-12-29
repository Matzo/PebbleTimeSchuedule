#pragma once
#include <pebble.h>

#define PEBBLE_SCREEN_WIDTH  144
#define PEBBLE_SCREEN_HEIGHT 168

//static void update_current_time_frame();
static void update_current_time_line();
//static void update_todays_events();
static float get_current_time_position();
static void scroll_to_current_time();
//static void draw_event(Layer *layer, Event event, GContext *ctx);

//static void appmsg_in_dropped(AppMessageResult appmsg, DictionaryResult dicresult, void *context);
//static void dictionary_requst(DictionaryResult reason, void *context);

static void send_refresh_notification();
static void clear_past_events();


typedef enum {
  STORAGE_KEY_DEVICE_TOKEN = 1
} STORAGE_KEY;
