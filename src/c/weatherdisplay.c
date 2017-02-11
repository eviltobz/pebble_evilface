#include <pebble.h>
#include "weatherdisplay.h"
#include "weatherparser.h"
#include "common.h"
#include "weatherlayer.h"
#include "debugout.h"

static void register_connection(void);
static void reregister_connection(void);

enum {
  KEY_WEATHER_MORNING = 1,
  KEY_WEATHER_EVENING = 2,
  KEY_WEATHER_CURRENT = 3,
  KEY_WEATHER_ERROR   = 99
};


static WeatherLayer *s_top_weather;
static BitmapLayer *s_separator;
static WeatherLayer *s_bottom_weather;

static GFont s_simple_font;
static TextLayer *s_weather_timestamp;

static char s_weather_timestamp_buffer[50];

static WeatherData s_current_weather_data;
static WeatherData s_morning_weather_data;
static WeatherData s_evening_weather_data;

static int s_attempt_number = 0;
static bool s_first_run = true; // app requesting weather before JS has initialised, then crashing.


void HACK_draw_next_weather(void) {
  LOG_DEBUG("CALLING HACK DRAW WEATHER!!!!");
  HACK_weatherlayer_drawnext(s_bottom_weather);
}

void flag_request_ended(void) {
  s_attempt_number = 0;
}

static bool update_due(struct tm *tick_time) {
  static const int REFRESH_DURATION = 30; // default to 30 mebe...
  static const int RETRY_LIMIT = 3;
  
  if(tick_time->tm_min % REFRESH_DURATION == 0) {
    s_attempt_number  = 1; // shouldn't this be 0?!?!?!
    return true;
  }
  
  if(s_attempt_number > 0 && s_attempt_number < RETRY_LIMIT + 1) {
    s_attempt_number++;
    return true;
  }
  
  if(s_attempt_number == RETRY_LIMIT) {
    debugout_log("Aborting request retries");
  }
  
  return false;
}

void do_update() {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
    log_requested();
}

void weatherdisplay_update(struct tm *tick_time) {
  if(update_due(tick_time) && !s_first_run)
    do_update();
  else
    s_first_run = false;
}

void weatherdisplay_reconnect() {
   if(!s_first_run) // && s_attempt_number > 1) - due to not being connected, there haven't been any attempts. bah!
    do_update();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  debugout_append("-Resp-");
  flag_request_ended();
  // Read first item
  Tuple *t = dict_read_first(iterator);
  
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case KEY_WEATHER_CURRENT:
        parse_weather(&s_current_weather_data, t->value->cstring);
        break;
      case KEY_WEATHER_MORNING:
        parse_weather(&s_morning_weather_data, t->value->cstring);
        break;
      case KEY_WEATHER_EVENING:
        parse_weather(&s_evening_weather_data, t->value->cstring);
        break;
      case KEY_WEATHER_ERROR:
        debugout_logerr("IN:", t->value->cstring);
        return;
      default:
        LOGF_ERROR("Key %d not recognized!", (int)t->key);
        break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  char log[4] = "";
  if(s_current_weather_data.is_valid) {
    /*snprintf(s_weather_timestamp_buffer, sizeof(s_weather_timestamp_buffer),
             "%s%s @ %s", s_current_weather_data.timestamp_hour, s_current_weather_data.timestamp_period, timestamp_minutes());
             */

    snprintf(s_weather_timestamp_buffer, sizeof(s_weather_timestamp_buffer),
             "%s", timestamp_minutes());
    text_layer_set_text(s_weather_timestamp, s_weather_timestamp_buffer);
    weatherlayer_update(s_top_weather, &s_current_weather_data);
    strcat(log, "C");
  } else {
    strcat(log, "c!!!");
  }
  
  if(s_morning_weather_data.is_valid){
    strcat(log, "M");
  } else {
    strcat(log, "m");
  }
    
  weatherlayer_clear(s_bottom_weather);
  if(s_evening_weather_data.is_valid) {
    strcat(log, "E");
    if (s_evening_weather_data.timestamp > s_current_weather_data.timestamp)
      weatherlayer_update(s_bottom_weather, &s_evening_weather_data);
  } else {
    strcat(log, "e");
  }
  log_received(log);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  debugout_logerrcode("IN:", reason);
  reregister_connection();
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  debugout_logerrcode("OUT:", reason);
  reregister_connection();
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  debugout_append("-OutSent-");
}

void create_simple_text(TextLayer **layer, int x, int y){
  (*layer) = build_textlayer(GRect(x,y,80,17), s_simple_font, GColorWhite, GTextAlignmentLeft);
}

void weatherdisplay_create(Window *window) {
  s_simple_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_10));
  create_simple_text(&s_weather_timestamp, 10, 154);
  
  s_top_weather = weatherlayer_create(window, 0);
  s_separator = build_bitmaplayer(GRect(2 + 3, 77, 70, 1), GColorWhite);
  s_bottom_weather = weatherlayer_create(window, 79);
  
  register_connection();
}

void weatherdisplay_delete(void) {
  text_layer_destroy(s_weather_timestamp);
  weatherlayer_delete(s_top_weather);
  weatherlayer_delete(s_bottom_weather);
  fonts_unload_custom_font(s_simple_font);
}


void register_connection() {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback); 
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void reregister_connection() {
  debugout_log("Re-registering Connections");
  app_message_deregister_callbacks();
  register_connection();
}
