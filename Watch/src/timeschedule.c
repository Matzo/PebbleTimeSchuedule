#include <pebble.h>
#include "events.h"

// APP_LOG_LEVEL_ERROR
// APP_LOG_LEVEL_WARNING
// APP_LOG_LEVEL_INFO
// APP_LOG_LEVEL_DEBUG
// APP_LOG_LEVEL_DEBUG_VERBOSE

/**
 * header
 */
static void update_current_time_frame();
static void update_current_time_line();
static void update_todays_events();
static float get_current_time_position();
static void scroll_to_current_time();
static void send_sample_app_messate();
//static void draw_event(Layer *layer, Event event, GContext *ctx);

static void appmsg_in_dropped(AppMessageResult appmsg, DictionaryResult dicresult, void *context);
//static void dictionary_requst(DictionaryResult reason, void *context);

static Window *window;
static AppTimer *timer;

static AppSync sync;
static uint8_t sync_buffer[256];

//enum EventKey {
//  EVENT_ID_KEY         = 0x1 << 0,  // TUPLE_CSTRING
//  EVENT_TITLE_KEY      = 0x1 << 1,  // TUPLE_CSTRING
//  EVENT_START_TIME_KEY = 0x1 << 2,  // TUPLE_INT
//  EVENT_END_TIME_KEY   = 0x1 << 3,  // TUPLE_INT
//};
//int loading = 0;

//static Event event_container;
static Event event_list[20];
//static List event_list;
static int event_index = 0;

//typedef struct {
//    ScrollLayer *scroll_layer;
//    TextLayer *current_time_layer;
//} WindowPropaties;

static ScrollLayer *scroll_layer;
static Layer *background_layer;
static TextLayer *current_time_layer;

static uint32_t TIMER_PERIOD = 1000 * 10;
static float ROW_HEIGHT = 40;
static float TIME_FONT_SIZE = 20;
static float START_OFFSET = 30;
static float EVENT_FONT_SIZE = 20;
static GPoint current_offset;
static bool is_keep_current = true;

/**
 * body
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //update_todays_events();
  scroll_to_current_time();
  is_keep_current = true;
}


/**
 * public methods
 */
static void update_todays_events() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "APP SYNC update_todays_event() %p", window);

//  time_t now = time(NULL);
//  Tuplet initial_values[] = {
//    TupletCString(EVENT_TITLE_KEY, "testapp"),
//    TupletInteger(EVENT_START_TIME_KEY, (int32_t) now),
//    TupletInteger(EVENT_END_TIME_KEY, (int32_t) now + 60*60)
//  };
//  app_sync_set(&sync, initial_values, ARRAY_LENGTH(initial_values));
}

static void update_current_time_frame() {
  GRect frame = layer_get_frame(text_layer_get_layer(current_time_layer));
  frame.origin.y = current_offset.y + ROW_HEIGHT;

  layer_set_frame(text_layer_get_layer(current_time_layer), frame);
}

static void scroll_to_current_time() {
//  static char time_text[] = "00:00:00"; // Needs to be static because it's used by the system later.
//  strftime(time_text, sizeof(time_text), "%T", localtime(&now));
//  text_layer_set_text(current_time_layer, time_text);

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "scroll_to_current_time");

  int16_t current_time_offset = (int16_t)get_current_time_position() - START_OFFSET;
//  GPoint target_offset = {0, -current_time_offset};
  current_offset.y = -current_time_offset;

  GPoint offset = scroll_layer_get_content_offset(scroll_layer);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "current offset x:%d y:%d", offset.x, offset.y);

  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
}
static void update_current_time_line() {
  GRect current_time_frame = layer_get_frame(text_layer_get_layer(current_time_layer));
  current_time_frame.origin.y = (int16_t)get_current_time_position() + START_OFFSET;
  layer_set_frame(text_layer_get_layer(current_time_layer), current_time_frame);

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "update_current_time_line() returned %d", current_time_frame.origin.y);

  if (is_keep_current) {
    scroll_to_current_time();
  }
}

static float get_time_position(uint32_t target_sec) {
  float one_day_height = ROW_HEIGHT * 24;
  float one_day_sec = 60*60*24;
  time_t now = time(NULL);
  float today = (float)now - (float)(now % (long)one_day_sec);
  float sec_from_today = target_sec - today;

  float result = ((sec_from_today / one_day_sec) * one_day_height);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "(today:%d) sec_from_today:%d / one_day_sec:%d * height:%d = result:%d", (int)today, (int)sec_from_today, (int)one_day_sec, (int)one_day_height, (int)result);
  return result;
}
static float get_current_time_position() {
  time_t now = time(NULL);
  return get_time_position((uint32_t)now);
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_offset.y += ROW_HEIGHT;
  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
  is_keep_current = false;
//  update_current_time_frame();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_offset.y -= ROW_HEIGHT;
  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
  is_keep_current = false;
//  update_current_time_frame();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
//  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
//  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);

  window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, down_click_handler);
//  window_long_click_subscribe(BUTTON_ID_UP, 200, up_click_handler, up_click_handler);
//  window_long_click_subscribe(BUTTON_ID_DOWN, 200, down_click_handler, down_click_handler);
}


/**
 * AppMessage
 */
static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  appmsg_in_dropped(app_message_error, dict_error, context);
//  dictionary_requst(dict_error, context);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

//static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
//  switch (key) {
//    case EVENT_TITLE_KEY:
//      //strncpy(event_list[event_index].title, new_tuple->value->cstring, sizeof(new_tuple->value->cstring));
//      strncpy(event_list[event_index].title, new_tuple->value->cstring, sizeof(event_list[event_index].title));
//      //strncpy(event_list[event_index].title, "event test", sizeof("event test"));
//      APP_LOG(APP_LOG_LEVEL_DEBUG, new_tuple->value->cstring);
//      APP_LOG(APP_LOG_LEVEL_DEBUG, event_list[event_index].title);
//      loading = loading | EVENT_TITLE_KEY;
//      break;
//    case EVENT_START_TIME_KEY:
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "event start time:%d", (int)new_tuple->value->int32);
//      //event_container.start_time = new_tuple->value->int32;
//      event_list[event_index].start_time = new_tuple->value->int32;
//      loading = loading | EVENT_START_TIME_KEY;
//      break;
//    case EVENT_END_TIME_KEY:
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "event end time:%d", (int)new_tuple->value->int32);
//      //event_container.end_time = new_tuple->value->int32;
//      event_list[event_index].end_time = new_tuple->value->int32;
//      loading = loading | EVENT_END_TIME_KEY;
//      break;
//  }
//
//  if (loading == (EVENT_TITLE_KEY | EVENT_START_TIME_KEY | EVENT_END_TIME_KEY)) {
//    // load finish
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "loading is completed");
//    loading = 0;
//
//    event_index++;
//    event_index = event_index % 20;
//
//    layer_mark_dirty(background_layer);
//  }
//}

static void draw_event(Layer *layer, Event *event, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  float left_margin = 30;
  float start_position = get_time_position(event->start_time);
  float end_position = get_time_position(event->end_time);

  // start line
  graphics_draw_line(ctx, GPoint(left_margin, start_position + START_OFFSET), GPoint(bounds.size.w, start_position + START_OFFSET));
  // end line
  graphics_draw_line(ctx, GPoint(left_margin, end_position + START_OFFSET), GPoint(bounds.size.w, end_position + START_OFFSET));
  // event mark
  graphics_fill_rect(ctx, GRect(left_margin + 2, start_position + START_OFFSET + 2, 4, end_position - start_position - 3), 0, GCornerNone);
  // draw event name
//  graphics_draw_text(ctx,
//      event->title,
//      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
//      GRect(left_margin + 8, start_position + START_OFFSET, bounds.size.w - left_margin - 8, end_position - start_position),
//      GTextOverflowModeWordWrap,
//      GTextAlignmentLeft,
//      NULL);

//  GBitmap *title_image = gbitmap_create_with_data(event->title_image);
//  GBitmap title_image;
//  title_image.bounds = GRect(0, 0, 100, 16),
//  title_image.row_size_bytes = 16;
//  title_image.addr = event->title_image;
  GBitmap title_image = (GBitmap) {
    .addr = event->title_image,
    .bounds = GRect(0, 0, 100, 16),
    .info_flags = 4096,
    .row_size_bytes = 16,
  };

  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, &title_image,
      GRect(left_margin + 8, start_position + START_OFFSET + 1, title_image.bounds.size.w, title_image.bounds.size.h));
//      GRect(left_margin + 8, start_position + START_OFFSET, title_image->bounds.size.w, title_image->bounds.size.h));
//      GRect(left_margin + 8, start_position + START_OFFSET, title_image.bounds.size.width, title_image.bounds.size.height));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "draw event title:%s start:%lu end:%lu", event->title, event->start_time, event->end_time);
}

static void draw_background(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  float left_margin = 30;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_text_color(ctx, GColorWhite);

  // vartical line
  GSize scroll_size = { bounds.size.w, ROW_HEIGHT*25 + bounds.size.h };
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  graphics_draw_line(ctx, GPoint(left_margin, 0), GPoint(left_margin, scroll_size.h));

  // hours
  int timeline_max = 25;
  char str[] = "00:00";
  for (int i = 0; i < timeline_max; i++) {
    snprintf(str, sizeof(str), "%02d:%02d", i, 0),
    graphics_draw_text(ctx,
        str,
        fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
        GRect(0, START_OFFSET + i * ROW_HEIGHT - TIME_FONT_SIZE/2 , left_margin, TIME_FONT_SIZE),
        GTextOverflowModeWordWrap,
        GTextAlignmentLeft,
        NULL);

    graphics_draw_line(ctx, GPoint(left_margin - 2, START_OFFSET + i * ROW_HEIGHT), GPoint(left_margin + 2, START_OFFSET + i * ROW_HEIGHT));
  }


//  if (0 < array_length(&event_list)) {
//    Event *e2 = array_get_item(&event_list, 0);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "draw_background event title:%s start:%lu end:%lu", e2->title, e2->start_time, e2->end_time);
//  }

  // event
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "draw event in background event_count:%d", array_length(&event_list));
  for (int i = 0; i < event_index; i++) {
    Event event = event_list[i];
//  for (int i = 0, len = array_length(&event_list); i < len; i++) {
//    Event *event = array_get_item(&event_list, i);
    // draw event
    draw_event(layer, &event, ctx);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "draw event title:%s start:%lu end:%lu", event.title, event.start_time, event.end_time);
  }
}


static void app_timer_handler(void *data) {
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "timer event");
  update_current_time_line();

  //send_sample_app_messate();

  timer = app_timer_register(TIMER_PERIOD, app_timer_handler, NULL);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // scroll
  GSize scroll_size = { bounds.size.w, ROW_HEIGHT*25 + bounds.size.h };
  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_content_size(scroll_layer, scroll_size);

  // current time
  GRect current_time_frame = { .origin = { 0, START_OFFSET }, .size = { bounds.size.w, 1 } };
  current_time_layer = text_layer_create(current_time_frame);
  text_layer_set_background_color(current_time_layer, GColorWhite);

//  char time_format[] = "";
//  text_layer_set_text(current_time_layer, strftime("", , , ));

  update_current_time_line();
  scroll_to_current_time();

  // background
  background_layer = layer_create((GRect) {.origin = {0,0}, .size = scroll_size });
  layer_set_update_proc(background_layer, draw_background);
  scroll_layer_add_child(scroll_layer, background_layer);

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(current_time_layer));
//  layer_add_child(window_layer, text_layer_get_layer(current_time_layer));

  window_set_background_color(window, GColorBlack);
//  scroll_layer_set_click_config_onto_window(scroll_layer, window);


  // AppTimer
  timer = app_timer_register(TIMER_PERIOD, app_timer_handler, NULL);


//  // AppSync
//  time_t now = time(NULL);
//  Tuplet initial_values[] = {
//    TupletCString(EVENT_TITLE_KEY, "testapp"),
//    TupletInteger(EVENT_START_TIME_KEY, (int32_t) now),
//    TupletInteger(EVENT_END_TIME_KEY, (int32_t) now + 60*60)
//  };
//  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
//      sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void window_unload(Window *window) {
  app_sync_deinit(&sync);

  text_layer_destroy(current_time_layer);
  layer_destroy(background_layer);
  scroll_layer_destroy(scroll_layer);

}



// commons utils
char *translate_appmsg_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}
char *translate_dict_error(DictionaryResult result) {
  switch (result) {
    case DICT_OK: return "DICT_OK";
    case DICT_NOT_ENOUGH_STORAGE: return "DICT_NOT_ENOUGH_STORAGE";
    case DICT_INVALID_ARGS: return "DICT_INVALID_ARGS";
    case DICT_INTERNAL_INCONSISTENCY: return "DICT_INTERNAL_INCONSISTENCY";
    case DICT_MALLOC_FAILED: return "DICT_MALLOC_FAILED";
    default: return "UNKNOWN ERROR";
  }
}
static void appmsg_in_dropped(AppMessageResult appmsg, DictionaryResult dicresult, void *context) {
   APP_LOG(APP_LOG_LEVEL_DEBUG, "In dropped: %i - %s Dictionary Result: %i - %s", appmsg, translate_appmsg_error(appmsg), dicresult, translate_dict_error(dicresult));
}
//static void dictionary_requst(DictionaryResult reason, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Dictionary Result: %i - %s", reason, translate_dict_error(reason));
//}


void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "app message was sent successfully!");
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failure, app message was not sent");
}


void in_received_handler(DictionaryIterator *received, void *context) {
  Event event = convert_tuple_to_event(received);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "[event]id:%s", event.id);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "[event]title:%s", event.title);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "[event]start:%lu", event.start_time);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "[event]end:%lu", event.end_time);

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "/=SAME EVENT=====");
//  GBitmap *title_image = gbitmap_create_with_data(event.title_image);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "GRect (x,y)(w,h):(%d,%d),(%d,%d) row_size_bytes:%d", (int)title_image->bounds.origin.x, (int)title_image->bounds.origin.y, (int)title_image->bounds.size.w, (int)title_image->bounds.size.h, (int)title_image->row_size_bytes);
//  title_image->bounds = GRect(0, 0, 100, 16);
//  title_image->row_size_bytes = 16;
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "GRect (x,y)(w,h):(%d,%d),(%d,%d) row_size_bytes:%d", (int)title_image->bounds.origin.x, (int)title_image->bounds.origin.y, (int)title_image->bounds.size.w, (int)title_image->bounds.size.h, (int)title_image->row_size_bytes);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, " =SAME EVENT=====/");


  // check event which is unique
  bool is_event_exists = false;
  for (int i = 0; i < event_index; i++) {
    Event exists_event = event_list[i];

//    APP_LOG(APP_LOG_LEVEL_DEBUG, "/=SAME EVENT=====");
//    GBitmap *title_image = gbitmap_create_with_data(event.title_image);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "GRect (x,y)(w,h):(%d,%d),(%d,%d)", (int)title_image->bounds.origin.x, (int)title_image->bounds.origin.y, (int)title_image->bounds.size.w, (int)title_image->bounds.size.h);
//    APP_LOG(APP_LOG_LEVEL_DEBUG, " =SAME EVENT=====/");

    if (strcmp(event.id, exists_event.id) == 0) {
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "/=SAME EVENT=====");
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "new.id:%s", event.id);
//      APP_LOG(APP_LOG_LEVEL_DEBUG, "exs.id:%s", exists_event.id);
//      APP_LOG(APP_LOG_LEVEL_DEBUG, " =SAME EVENT=====/");

      is_event_exists = true;
      break;
    }
  }
  if (!is_event_exists) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Append Event:%d", event_index);
    event_list[event_index] = event;
    event_index++;
    layer_mark_dirty(background_layer);
  }


//  // LOG
//  for (int i = 0; i < event_index; i++) {
//    Event _e = event_list[i];
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "!!! event:%d index:%d title:%s start:%lu end:%lu", i, event_index, _e.title, _e.start_time, _e.end_time);
//  }


//  array_add_item(&event_list, &event);

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event 1");
//  Event *e = event_list.value;
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event 2");
//  List *e_list = NULL;
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event 3");
//

//  Event *e2 = array_get_first(&event_list);
//  Event *e2 = array_get_item(&event_list, 0);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event title:%s start:%lu end:%lu", e2->title, e2->start_time, e2->end_time);

//  while ((e_list = e_list->next) != NULL) {
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event 4");
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "in_received_handler event title:%s start:%lu end:%lu", e->title, e->start_time, e->end_time);
//    e = e_list->value;
//  }



  // incoming message received
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "app message was received successfully! dict size:%d", (int)dict_size(received));
//
//  Tuple *event_id_tuple = dict_find(received, EVENT_ID_KEY);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "event_id:%s", event_id_tuple->value->cstring);
//
//  Tuple *event_title_tuple = dict_find(received, EVENT_TITLE_KEY);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "event_title:%s", event_title_tuple->value->cstring);
//
//  Tuple *event_start_tuple = dict_find(received, EVENT_START_TIME_KEY);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "start:%lu", event_start_tuple->value->int32);
//
//  Tuple *event_end_tuple = dict_find(received, EVENT_END_TIME_KEY);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "end:%lu", event_end_tuple->value->int32);

//  Tuple *tuple = NULL;
//  while ((tuple = dict_read_next(received)) != NULL) {
//
//  }
}


void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failure, app message was not received");
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = app_message_inbox_size_maximum();
  const uint32_t outbound_size = app_message_outbox_size_maximum();
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox size:%d outbox size:%d", (int)inbound_size, (int)outbound_size);
  app_message_open(inbound_size, outbound_size);
}

static void send_sample_app_messate() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, 42);
  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
}



static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  current_offset = GPoint(0,0);
  window_stack_push(window, animated);

  //update_todays_events();

  // AppMessage
//  const int inbound_size = 64;
//  const int outbound_size = 16;
//  app_message_open(inbound_size, outbound_size);


  app_message_init();
//  send_sample_app_messate();
}

static void deinit(void) {
  window_destroy(window);

  for (int i = 0; i < event_index; i++) {
    Event event = event_list[i];
    free(event.id);
    free(event.title);
    free(event.title_image);
  }
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

// vim: set ts=2 sw=2 sts=2:
