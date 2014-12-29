#include "events.h"

AppMessageType app_message_type(DictionaryIterator *itr) {
  AppMessageType msgType = APP_MESSAGE_TYPE_EVENT; // Default type is Event
  Tuple *msg_type_tuple = dict_find(itr, APP_MESSAGE_TYPE_KEY);

  if (msg_type_tuple != NULL) {
    msgType = msg_type_tuple->value->int16;
  }

  return msgType;
}

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
//  char *event_title;
  uint8_t *event_title_image;

  Tuple *event_id_tuple = dict_find(itr, EVENT_ID_KEY);
//  Tuple *event_title_tuple = dict_find(itr, EVENT_TITLE_KEY);
  Tuple *event_title_image_tuple = dict_find(itr, EVENT_TITLE_IMAGE_KEY);
  Tuple *event_start_tuple = dict_find(itr, EVENT_START_TIME_KEY);
  Tuple *event_end_tuple = dict_find(itr, EVENT_END_TIME_KEY);
//  Tuple *event_image_width_tuple = dict_find(itr, EVENT_IMAGE_WIDTH);
//  Tuple *event_image_height_tuple = dict_find(itr, EVENT_IMAGE_HEIGHT);
//  Tuple *event_image_row_size_bytes_tuple = dict_find(itr, EVENT_IMAGE_ROW_SIZE_BYTES);
//  Tuple *event_image_info_flags_tuple = dict_find(itr, EVENT_IMAGE_INFO_FlAGS);

  event_id = malloc(event_id_tuple->length);
  strncpy(event_id, event_id_tuple->value->cstring, event_id_tuple->length);
  event.id = event_id;

//  event_title = malloc(event_title_tuple->length);
//  strncpy(event_title, event_title_tuple->value->cstring, event_title_tuple->length);
//  event.title = event_title;

  event_title_image = malloc(event_title_image_tuple->length);
  memcpy(event_title_image, event_title_image_tuple->value->data, event_title_image_tuple->length);
  event.title_image = event_title_image;

  event.start_time = event_start_tuple->value->int32;
  event.end_time = event_end_tuple->value->int32;

//  event.image_width = event_image_width_tuple->value->int16;
//  event.image_height = event_image_height_tuple->value->int16;
//  event.image_row_size_bytes = event_image_row_size_bytes_tuple->value->int16;
//  event.image_info_flags = event_image_info_flags_tuple->value->int16;
  return event;
}

char* ios_devtoken(DictionaryIterator *itr) {
  char *device_token;
  Tuple *device_token_tuple = dict_find(itr, IOS_DEVIDE_TOKEN_KEY);

  device_token = malloc(device_token_tuple->length);
  strncpy(device_token, device_token_tuple->value->cstring, device_token_tuple->length);

  return device_token;
}
