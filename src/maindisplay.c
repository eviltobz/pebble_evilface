// time&date, status
#include <pebble.h>
#include "timedisplay.h"
#include "statusdisplay.h"
#include "common.h"
#include "debugout.h"

  
  
static GFont s_time_font;
static GFont s_date_font;
static TextLayer *s_hours;
static TextLayer *s_mins;
static TextLayer *s_date;
static BitmapLayer *s_date_background;

//static statusdisplay *s_status;

void maindisplay_updatetime(struct tm *tick_time) {
  static char hours[3], mins[3];
  strftime(hours, 3, "%I", tick_time);
  strftime(mins, 3, "%M", tick_time);
  
  text_layer_set_text(s_hours, hours);
  text_layer_set_text(s_mins, mins);
  
  static char date[12];
  strftime(date, sizeof(date), "%a%d%b", tick_time);
  text_layer_set_text(s_date, date);
}
void maindisplay_update_battery(BatteryChargeState state) {
  statusdisplay_update_battery(state);
}

void maindisplay_update_connection(bool connected) {
  statusdisplay_update_connection(connected);
}

void maindisplay_create(Window *window){
  // Time & Date
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_85));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  
  s_date_background = build_bitmaplayer(GRect(85, 133, 56, 18), GColorWhite);
  s_date  = build_textlayer(GRect(82,132,62,21), s_date_font, GColorBlack, GTextAlignmentCenter);
  
  s_hours = build_default_textlayer(GRect(74, -22, 74, 93), s_time_font);
  s_mins  = build_default_textlayer(GRect(74, 43, 74, 93), s_time_font);

  // Status
  statusdisplay_create(window);
  
  maindisplay_updatetime(get_current_time());
}

void maindisplay_delete(void){
  bitmap_layer_destroy(s_date_background);
  text_layer_destroy(s_hours);
  text_layer_destroy(s_mins);
  text_layer_destroy(s_date);
  fonts_unload_custom_font(s_time_font);

  statusdisplay_delete();
}






