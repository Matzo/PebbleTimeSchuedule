#include <math.h>
#include "timeschedule.h"
#include "pebble_utils.h"
#include "events.h"

// APP_LOG_LEVEL_ERROR
// APP_LOG_LEVEL_WARNING
// APP_LOG_LEVEL_INFO
// APP_LOG_LEVEL_DEBUG
// APP_LOG_LEVEL_DEBUG_VERBOSE

static Window *window;
static AppTimer *timer;

//static Event event_container;
static Event event_list[20];
//static List event_list;
static int event_index = 0;

static ScrollLayer *scroll_layer;
static Layer *background_layer;
static TextLayer *current_time_layer;

static uint32_t TIMER_PERIOD = 1000 * 10;
static float ROW_HEIGHT = 40;
static float DAY_MAX_HEIGHT = 1000; // ROW_HEIGHT * 25
static float TIME_FONT_SIZE = 20;
static float START_OFFSET = 20;
static GPoint current_offset;
static bool is_keep_current = true;
static long last_day_num = 0;


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //update_todays_events();
  scroll_to_current_time();
  is_keep_current = true;
}

static void scroll_to_current_time() {
  int16_t current_time_offset = (int16_t)get_current_time_position() - START_OFFSET;
  current_offset.y = - current_time_offset;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  float MAX_OFFSET = DAY_MAX_HEIGHT - bounds.size.h;
  if (current_offset.y < -MAX_OFFSET) {
    current_offset.y = -MAX_OFFSET;
  }

  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
}
static void update_current_time_line() {
  GRect current_time_frame = layer_get_frame(text_layer_get_layer(current_time_layer));
  current_time_frame.origin.y = (int16_t)get_current_time_position() + START_OFFSET;
  layer_set_frame(text_layer_get_layer(current_time_layer), current_time_frame);

  if (is_keep_current) {
    scroll_to_current_time();
  }
}

static long get_time_position(uint32_t target_sec) {
  long one_day_height = ROW_HEIGHT * 24;
  long one_day_sec = 60*60*24;
  time_t now = time(NULL);
  long today = (long)(now - (now % one_day_sec));
  long sec_from_today = target_sec - today;

  long result = (long)round((((float)sec_from_today / (float)one_day_sec) * (float)one_day_height));
  return result;
}

static float get_current_time_position() {
  time_t now = time(NULL);
  return get_time_position((uint32_t)now);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_offset.y += ROW_HEIGHT;
  if (0 < current_offset.y) {
    current_offset.y = 0;
  }
  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
  is_keep_current = false;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  float MAX_OFFSET = DAY_MAX_HEIGHT - bounds.size.h;
  current_offset.y -= ROW_HEIGHT;
  if (current_offset.y < -MAX_OFFSET) {
    current_offset.y = -MAX_OFFSET;
  }
  scroll_layer_set_content_offset(scroll_layer, current_offset, true);
  is_keep_current = false;
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, down_click_handler);
}

static void draw_event(Layer *layer, Event *event, GRect rect, GContext *ctx) {
  // event rect
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, rect, 0, GCornerNone);
  graphics_draw_rect(ctx, rect);

  // event mark
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(rect.origin.x + 2, rect.origin.y + 2, 4, rect.size.h - 4), 0, GCornerNone);

  // draw event name
// #if def PBL_PLATFORM_BASALT
//   GBitmap title_image = gbitmap_create_blank()
// #else
//   GBitmap title_image = (GBitmap) {
//     .addr = event->title_image,
//     .bounds = GRect(0, 0, 96, 8),
//     .info_flags = 4096,
//     .row_size_bytes = 12,
//   };
// #endif
  GBitmap *title_image = gbitmap_create_with_data(event->title_image);
//  GBitmap title_image = gbitmap_create_blank(GSize(96, 8), GBitmapFormat1Bit);
//  title_image->info_flags = 4096;
//  title_image->row_size_bytes = 12;
//  title_image->addr = event->title_image;


  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, title_image,
      GRect(rect.origin.x + 8, rect.origin.y + 4, gbitmap_get_bounds(title_image).size.w, gbitmap_get_bounds(title_image).size.h)
      );
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "draw event title:%s start:%lu end:%lu", event->title, event->start_time, event->end_time);
}

static void draw_background(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  float left_margin = 30;

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_text_color(ctx, GColorWhite);

  // vartical line
  GSize scroll_size = { bounds.size.w, DAY_MAX_HEIGHT };
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  graphics_draw_line(ctx, GPoint(left_margin, 0), GPoint(left_margin, scroll_size.h));

  // hours
  int timeline_max = 25;
  char str[] = "00:00";
  for (int i = 0; i < timeline_max; i++) {
    snprintf(str, sizeof(str), "%02d:%02d", i, 0),
    graphics_draw_text(ctx,
        str,
        fonts_get_system_font(FONT_KEY_GOTHIC_14),
        GRect(0, START_OFFSET + i * ROW_HEIGHT - TIME_FONT_SIZE/2 , left_margin, TIME_FONT_SIZE),
        GTextOverflowModeWordWrap,
        GTextAlignmentLeft,
        NULL);

    graphics_draw_line(ctx, GPoint(left_margin - 2, START_OFFSET + i * ROW_HEIGHT), GPoint(left_margin + 2, START_OFFSET + i * ROW_HEIGHT));
  }

  // event
  float last_event_start_position = 0;
  float last_event_end_position = 0;
  float event_pos_x = left_margin;
  for (int i = 0; i < event_index; i++) {
    Event event = event_list[i];
    float start_position = get_time_position(event.start_time);
    float end_position = get_time_position(event.end_time);

    if (start_position < last_event_end_position) {
      if (start_position - last_event_start_position < 15) {
        event_pos_x += 40;
      } else {
        event_pos_x += 10;
      }
    } else {
      event_pos_x = left_margin;
    }

    GRect event_rect = GRect(event_pos_x, start_position + START_OFFSET, bounds.size.w - event_pos_x, end_position - start_position);

    draw_event(layer, &event, event_rect, ctx);

    last_event_start_position = start_position;
    last_event_end_position = end_position;
  }
}

static void clear_past_events() {
  long one_day_sec = 60*60*24;
  time_t now = time(NULL);
  long today = (long)(now - (now % one_day_sec));

  Event current_event_list[20];
  int current_event_index = 0;
  for (int i = 0; i < event_index; i++) {
    Event event = event_list[i];
    if (today <= (long)event.end_time) {
      current_event_list[current_event_index++] = event;
    }
  }

  for (int i = 0; i < event_index; i++) {
    event_list[i] = current_event_list[i];
  }
  event_index = current_event_index;
}

/***** Functions for Application *****/
void get_initial_schedule() {

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

//   Tuplet t = TupletCString(IOS_DEVIDE_TOKEN_KEY, devtoken);
//   dict_write_tuplet(iter, &t);

  app_message_outbox_send();
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "app message was sent successfully!");
}

static void send_refresh_notification() {
  char *devtoken = malloc(PERSIST_DATA_MAX_LENGTH);
  persist_read_string(STORAGE_KEY_DEVICE_TOKEN, devtoken, PERSIST_DATA_MAX_LENGTH);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  Tuplet t = TupletCString(IOS_DEVIDE_TOKEN_KEY, devtoken);
  dict_write_tuplet(iter, &t);

  app_message_outbox_send();
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "app message was sent successfully!");
}

void save_device_token(char *device_token) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "devide token:%s", device_token);
  persist_write_string(STORAGE_KEY_DEVICE_TOKEN, device_token);
//
//
//  char *saved = malloc(PERSIST_DATA_MAX_LENGTH);
//
//  persist_read_string(STORAGE_KEY_DEVICE_TOKEN, saved, PERSIST_DATA_MAX_LENGTH);
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "saved devide token:%s", saved);
}
/***** /Functions for Application *****/

/***** Event Handers *****/
static void app_timer_handler(void *data) {
  update_current_time_line();

  time_t now = time(NULL);
  long one_day_sec = 60*60*24;
  long current_days_num = (long)(now / one_day_sec);
  if (last_day_num != current_days_num) {
    clear_past_events();
//    send_refresh_notification();
    last_day_num = current_days_num;
  }

  timer = app_timer_register(TIMER_PERIOD, app_timer_handler, NULL);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // scroll
  GSize scroll_size = { bounds.size.w, DAY_MAX_HEIGHT };
  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_content_size(scroll_layer, scroll_size);

  // current time
  GRect current_time_frame = { .origin = { 0, START_OFFSET }, .size = { bounds.size.w, 1 } };
  current_time_layer = text_layer_create(current_time_frame);
  text_layer_set_background_color(current_time_layer, GColorWhite);

  update_current_time_line();
  scroll_to_current_time();

  // background
  background_layer = layer_create((GRect) {.origin = {0,0}, .size = scroll_size });
  layer_set_update_proc(background_layer, draw_background);
  scroll_layer_add_child(scroll_layer, background_layer);

  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(current_time_layer));

  window_set_background_color(window, GColorBlack);

  // AppTimer
  timer = app_timer_register(TIMER_PERIOD, app_timer_handler, NULL);
}

static void window_unload(Window *window) {
  text_layer_destroy(current_time_layer);
  layer_destroy(background_layer);
  scroll_layer_destroy(scroll_layer);
}

// AppMessage Handler

//static void appmsg_in_dropped(AppMessageResult appmsg, DictionaryResult dicresult, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "In dropped: %i - %s Dictionary Result: %i - %s", appmsg, translate_appmsg_error(appmsg), dicresult, translate_dict_error(dicresult));
//}

void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "app message was sent successfully!");
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failure, app message was not sent (ERROR_CODE=%d)", reason);
}


void in_received_handler(DictionaryIterator *received, void *context) {
  AppMessageType msgType = app_message_type(received);

  if (msgType == APP_MESSAGE_TYPE_EVENT) {
    Event event = convert_tuple_to_event(received);

    // check event which is unique
    bool is_event_exists = false;
    for (int i = 0; i < event_index; i++) {
      Event exists_event = event_list[i];

      if (strcmp(event.id, exists_event.id) == 0) {
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
  } else if (msgType == APP_MESSAGE_TYPE_DEVICETOKEN) {
    save_device_token(ios_devtoken(received));
  }
}


void in_dropped_handler(AppMessageResult reason, void *context) {
  // incoming message dropped
  APP_LOG(APP_LOG_LEVEL_DEBUG, "failure, app message was not received (ERROR_CODE=%d)", reason);
}

/***** /Event Handers *****/

/***** Application Initialization *****/
static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = app_message_inbox_size_maximum();
  const uint32_t outbound_size = app_message_outbox_size_maximum();
  app_message_open(inbound_size, outbound_size);
//  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
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

  app_message_init();
  get_initial_schedule();
  //send_refresh_notification();
}

static void deinit(void) {
  window_destroy(window);

  for (int i = 0; i < event_index; i++) {
    Event event = event_list[i];
    free(event.id);
    free(event.title_image);
  }
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
/***** Application Initialization *****/

// vim: set ts=2 sw=2 sts=2:
