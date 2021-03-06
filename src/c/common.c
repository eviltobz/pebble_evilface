#include <pebble.h>
#include "common.h"



struct tm* get_current_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  return tick_time;
}

static char* timestamp(char* format){
  static char time_string[10];
  strftime(time_string, sizeof(time_string), format, get_current_time());
  return time_string;
}

char* timestamp_minutes()
{
  return timestamp("%I:%M");
}
char* timestamp_seconds()
{
  return timestamp("%I:%M:%S");
}

static Layer *s_root_layer;
void set_root_layer(Window *window){
  s_root_layer = window_get_root_layer(window);
}

TextLayer* build_textlayer(GRect bounds, GFont font, GColor colour, GTextAlignment alignment){
  TextLayer *local = text_layer_create(bounds);
  text_layer_set_background_color(local, GColorClear);
  text_layer_set_text_color(local, colour);
  text_layer_set_font(local, font);
  text_layer_set_text_alignment(local, alignment);
  layer_add_child(s_root_layer, (Layer *)local);
  text_layer_set_text(local, "");
  return local;
}

TextLayer* build_default_textlayer(GRect bounds, GFont font){
  return build_textlayer(bounds, font, GColorWhite, GTextAlignmentCenter);
}

BitmapLayer* build_bitmaplayer(GRect bounds, GColor colour) {
  BitmapLayer *local = bitmap_layer_create(bounds);
  bitmap_layer_set_background_color(local, colour);
  bitmap_layer_set_compositing_mode(local, GCompOpSet);
  layer_add_child(s_root_layer, (Layer *)local);
  return local;
}

// see http://stackoverflow.com/questions/21150193/logging-enums-on-the-pebble-watch
char *translate_error(AppMessageResult result) {
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