#pragma once
#include <pebble.h>

enum EventKey {
  EVENT_ID_KEY         = 1 << 0,  // TUPLE_CSTRING
  EVENT_TITLE_KEY      = 1 << 1,  // TUPLE_CSTRING
  EVENT_TITLE_IMAGE_KEY= 1 << 2,  // TUPLE_BYTE_ARRAY
  EVENT_START_TIME_KEY = 1 << 3,  // TUPLE_INT
  EVENT_END_TIME_KEY   = 1 << 4,  // TUPLE_INT
};

typedef struct {
  char *id;
  char *title;
  uint8_t *title_image;
  uint32_t start_time;
  uint32_t end_time;
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
