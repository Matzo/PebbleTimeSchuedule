#pragma once
#include <pebble.h>

enum EventKey {
  EVENT_ID_KEY         = 0, // TUPLE_CSTRING
//  EVENT_TITLE_KEY      = 1, // TUPLE_CSTRING
  EVENT_TITLE_IMAGE_KEY= 2, // TUPLE_BYTE_ARRAY
  EVENT_START_TIME_KEY = 3, // TUPLE_INT
  EVENT_END_TIME_KEY   = 4, // TUPLE_INT
//  EVENT_IMAGE_WIDTH    = 5, // TUPLE_INT
//  EVENT_IMAGE_HEIGHT   = 6, // TUPLE_INT
//  EVENT_IMAGE_ROW_SIZE_BYTES = 7, // TUPLE_INT
//  EVENT_IMAGE_INFO_FlAGS     = 8, // TUPLE_INT
};

typedef struct {
  char *id;
//  char *title;
  uint8_t *title_image;
  uint32_t start_time;
  uint32_t end_time;
//  uint16_t image_width;
//  uint16_t image_height;
//  uint16_t image_row_size_bytes;
//  uint16_t image_info_flags;
} Event;

typedef struct {
  void *value;
  void *next;
} List;

void array_add_item(List *list, void *item);
void* array_get_item(List *list, int index);
void* array_get_first(List *list);
void* array_get_last(List *list);
int array_length(List *list);
Event convert_tuple_to_event(DictionaryIterator *itr);
