#include <pebble.h>
#include "weatherdisplay.h"
#include "weatherparser.h"
#include "common.h"
#include "error.h"
#include "weatherlayer.h"
#include "debugout.h"

enum {
  KEY_WEATHER_MORNING = 1,
  KEY_WEATHER_EVENING = 2,
  KEY_WEATHER_CURRENT = 3
};

static bool initialising = true;
static bool s_request_outstanding = false;
  

static BitmapLayer *s_separator;


static GFont *s_simple_font;
static TextLayer *s_weather_timestamp;
static char s_weather_timestamp_buffer[50];

static WeatherData s_current_weather_data;
static WeatherData s_morning_weather_data;
static WeatherData s_evening_weather_data;

static WeatherLayer *s_top_weather;
static WeatherLayer *s_bottom_weather;

void HACK_draw_next_weather(void) {
  LOG_DEBUG("CALLING HACK DRAW WEATHER!!!!");
  //HACK_weatherlayer_drawnext(s_bottom_weather);
  /*
  s_REQ_IN_PROCESS = !s_REQ_IN_PROCESS;
  if(s_REQ_IN_PROCESS)
    debug_append_line("HACK LINE");
  else
    debug_append_line("hack line");
  */
}

static bool update_due(struct tm *tick_time) {
  static const int REFRESH_DURATION = 30; // default to 30 mebe...
  static const int RETRY_DURATION = 5;
  
  if(tick_time->tm_min % REFRESH_DURATION == 0) {
    s_request_outstanding = true;
    return true;
  }
  if(s_request_outstanding && (tick_time->tm_min % RETRY_DURATION == 5)) 
    return true;
  
  return false;
}

void weatherdisplay_update(struct tm *tick_time) {
  
  static bool first_run = true; // app requesting weather before JS has initialised, then crashing.
  if(update_due(tick_time) && !first_run) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    //LOG_INFO("sending weather request");
    
    app_message_outbox_send();
    log_requested();
  }
  else
    first_run = false;
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  //LOG_DEBUG("Received weather data..");
  debugout_append("-Resp-");
  s_request_outstanding = false;
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
      default:
        LOGF_ERROR("Key %d not recognized!", (int)t->key);
        break;
    }
    // Look for next item
    t = dict_read_next(iterator);
  }
  char log[4] = "";
  if(s_current_weather_data.is_valid) {
    snprintf(s_weather_timestamp_buffer, sizeof(s_weather_timestamp_buffer), "%s%s @ %s", s_current_weather_data.timestamp_hour, s_current_weather_data.timestamp_period, timestamp_minutes());
    text_layer_set_text(s_weather_timestamp, s_weather_timestamp_buffer);
    weatherlayer_update(s_top_weather, &s_current_weather_data);
    strcat(log, "C");
  } else {
    strcat(log, "c");
  }
  
  if(s_morning_weather_data.is_valid){
    strcat(log, "M");
  } else {
    strcat(log, "m");
  }
    
  if(s_evening_weather_data.is_valid) {
    strcat(log, "E");
    if (s_evening_weather_data.timestamp_hour >= s_current_weather_data.timestamp_hour || 
        s_evening_weather_data.timestamp_period != s_current_weather_data.timestamp_period) {
      weatherlayer_update(s_bottom_weather, &s_evening_weather_data);
    }
  } else {
    weatherlayer_clear(s_bottom_weather);
    strcat(log, "e");
  }
  log_received(log);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  debugout_logerrcode("IN:", reason);
  //log_inbox_dropped(translate_error(reason));
  //LOGF_ERROR("Message dropped! %s", translate_error(reason));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  debugout_logerrcode("OUT:", reason);
  //log_outbox_failed(translate_error(reason));
  //LOGF_ERROR("Outbox send failed! %s", translate_error(reason));
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  //LOG_DEBUG("Outbox send success!");
  debugout_append("-OutSent-");
}

void create_simple_text(TextLayer **layer, int x, int y){
  (*layer) = build_textlayer(GRect(x,y,80,17), s_simple_font, GColorWhite, GTextAlignmentLeft);
}

void weatherdisplay_create(Window *window) {
  s_simple_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_10));

  create_simple_text(&s_weather_timestamp, 10, 154);
  
  
  s_separator = build_bitmaplayer(GRect(2, 77, 70, 1), GColorWhite);
  
  s_top_weather = weatherlayer_create(window, 0);
  s_bottom_weather = weatherlayer_create(window, 79);
  
  
  initialising = false;
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback); 
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void weatherdisplay_delete(void) {
  text_layer_destroy(s_weather_timestamp);
  weatherlayer_delete(s_top_weather);
  weatherlayer_delete(s_bottom_weather);
  fonts_unload_custom_font(s_simple_font);
}