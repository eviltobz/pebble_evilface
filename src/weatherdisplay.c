#include <pebble.h>
#include "weatherdisplay.h"

enum {
  KEY_TEMPERATURE = 0,
  KEY_CONDITIONS = 1
};

static TextLayer *s_weather;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


void weatherdisplay_create(Window *window) {
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_weather = text_layer_create(GRect(0, 0, 90, 21));
  text_layer_set_background_color(s_weather, GColorClear);
  text_layer_set_text_color(s_weather, GColorWhite);
  text_layer_set_font(s_weather, font);
  layer_add_child(window_get_root_layer(window), (Layer *)s_weather);
  
  //text_layer_set_text(s_weather, "Loading Weather");
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback); 
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void weatherdisplay_delete(void) {
  text_layer_destroy(s_weather);
}