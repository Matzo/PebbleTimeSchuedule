#include "events.h"

void array_add_item(List *list, void* item) {
  if (list->value == NULL) {
    list->value = item;
  } else {
    List *last_item = array_get_last(list);

    List new_item;
    new_item.value = &item;
    last_item->next = &new_item;
  }
}

void* array_get_first(List *list) {
  return list->value;
}

void* array_get_last(List *list) {
  void *value = list->value;
  while ((list = list->next) != NULL) {
    value = list->value;
  }
  return value;
}

int array_length(List *list) {
  int count = (list->value != NULL) ? 1 : 0;
  while ((list = list->next) != NULL) {
    count++;
  }
  return count;
}

void* array_get_item(List *list, int index) {
  int count = 0;
  void *value = list->value;
  while ((list = list->next) != NULL) {
    if (count == index) {
      break;
    }
    value = list->value;
    count++;
  }
  return value;
}

Event convert_tuple_to_event(DictionaryIterator *itr) {
  Event event;
  char *event_id;
  char *event_title;
  uint8_t *event_title_image;
  uint32_t start_time;
  uint32_t end_time;

  Tuple *event_id_tuple = dict_find(itr, EVENT_ID_KEY);
  Tuple *event_title_tuple = dict_find(itr, EVENT_TITLE_KEY);
  Tuple *event_title_image_tuple = dict_find(itr, EVENT_TITLE_IMAGE_KEY);
  Tuple *event_start_tuple = dict_find(itr, EVENT_START_TIME_KEY);
  Tuple *event_end_tuple = dict_find(itr, EVENT_END_TIME_KEY);

  event_id = malloc(event_id_tuple->length);
  strncpy(event_id, event_id_tuple->value->cstring, event_id_tuple->length);
  event.id = event_id;

  event_title = malloc(event_title_tuple->length);
  strncpy(event_title, event_title_tuple->value->cstring, event_title_tuple->length);
  event.title = event_title;

  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "img size:%d", event_title_image_tuple->length);

  event_title_image = malloc(event_title_image_tuple->length);
  memcpy(event_title_image, event_title_image_tuple->value->data, event_title_image_tuple->length);
  event.title_image = event_title_image;

  event.start_time = event_start_tuple->value->int32;
  event.end_time = event_end_tuple->value->int32;

  return event;
}
