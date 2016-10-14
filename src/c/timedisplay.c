#include <pebble.h>
#include "timedisplay.h"
#include "common.h"
#include "debugout.h"

  // dirty haxx
//#include "weatherdisplay.h"
  
  
static GFont s_time_font;
static GFont s_date_font;
static TextLayer *s_hours;
static TextLayer *s_mins;
static TextLayer *s_date;
static BitmapLayer *s_date_background;

void timedisplay_update(struct tm *tick_time) {
  static char hours[3], mins[3];
  strftime(hours, 3, "%I", tick_time);
  strftime(mins, 3, "%M", tick_time);
  
  text_layer_set_text(s_hours, hours);
  text_layer_set_text(s_mins, mins);
  
  static char date[12];
  strftime(date, sizeof(date), "%a%d%b", tick_time);
  text_layer_set_text(s_date, date);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  /*
  static char hours[3], mins[3];
  strftime(hours, 3, "%I", tick_time);
  strftime(mins, 3, "%M", tick_time);
  
  text_layer_set_text(s_hours, hours);
  text_layer_set_text(s_mins, mins);
  
  static char date[12];
  strftime(date, sizeof(date), "%a%d%b", tick_time);
  text_layer_set_text(s_date, date);
  */
  timedisplay_update(tick_time);
  
  // This says we should pull out the tick handler as a separate thing to 
  // do updates to time & date & weather & whatever else...
  // infact, mebe just a single event handler point that spins out to the
  // different areas? so then the weather display could just take relevant
  // weather info struct as an arg
  //weatherdisplay_update(tick_time);
  // especially as this can start trying to draw to the screen, in elements
  // which may not yet have been created!
}

static void SECONDHACK_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  tick_handler(tick_time, units_changed);
  //HACK_draw_next_weather();
  char line[50];
  static int count = 1;
  FORMAT_STRING(line, "%d - Hacky log info to test", count++);
  debugout_log(line);
}


void timedisplay_create(Window *window){
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_85));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_76));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  s_date_background = build_bitmaplayer(GRect(85, 133, 56, 18), GColorWhite);
  s_date  = build_textlayer(GRect(82,132,62,21), s_date_font, GColorBlack, GTextAlignmentCenter);
  
  s_hours = build_default_textlayer(GRect(74, -22, 74, 93), s_time_font);
  s_mins  = build_default_textlayer(GRect(74, 43, 74, 93), s_time_font);
  
  timedisplay_update(get_current_time());
  //tick_handler(get_current_time(), MINUTE_UNIT);
  
  // delegated to eventhandler
  //tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  //tick_timer_service_subscribe(SECOND_UNIT, SECONDHACK_tick_handler);
}

void timedisplay_delete(void){
  bitmap_layer_destroy(s_date_background);
  text_layer_destroy(s_hours);
  text_layer_destroy(s_mins);
  text_layer_destroy(s_date);
  fonts_unload_custom_font(s_time_font);
}
