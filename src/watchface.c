#include <pebble.h>
#include "watchface.h"
#include "timedisplay.h"
#include "weatherdisplay.h"
#include "statusdisplay.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_18_bold;
static BitmapLayer *s_debug_grid_v_half;
static BitmapLayer *s_debug_grid_h_half;
static Layer *s_layer_1;
static TextLayer *s_textlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  // s_debug_grid_v_half
  s_debug_grid_v_half = bitmap_layer_create(GRect(71, 0, 2, 167));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_debug_grid_v_half);
  
  // s_debug_grid_h_half
  s_debug_grid_h_half = bitmap_layer_create(GRect(0, 83, 143, 2));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_debug_grid_h_half);
  
  // s_layer_1
  s_layer_1 = layer_create(GRect(21, 19, 40, 40));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_layer_1);
  
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(20, 20, 100, 84));
  text_layer_set_text(s_textlayer_1, "Text layer");
  text_layer_set_font(s_textlayer_1, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_debug_grid_v_half);
  bitmap_layer_destroy(s_debug_grid_h_half);
  layer_destroy(s_layer_1);
  text_layer_destroy(s_textlayer_1);
}
// END AUTO-GENERATED UI CODE


/*
void update_time(struct tm *tick_time){
  
  static char date_buffer1[] = "XXX, XX/XX/XXXX";
  static char date_buffer2[] = "XXX, XX/XX/XXXX";
  strftime(date_buffer1, sizeof("XX"), "%d", tick_time);
  strftime(date_buffer2, sizeof(date_buffer2), "%a - %b", tick_time);
  datedisplay_update(date_buffer1, date_buffer2);
  
  static char time_string[5];
  strftime(time_string, 5, "%I%M", tick_time);
  //strftime(time_string, 5, "%S%S", tick_time);
  timedisplay_update(time_string);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void DEBUGGY_second_handler(struct tm *tick_time, TimeUnits units_changed) {
  // DEBUGGY STUFF HERE
  
  
  
  // FALLTHROUGH TO ACTUAL TIME UPDATE
  tick_handler(tick_time, units_changed);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
  timedisplay_delete();
  datedisplay_delete();
  statusdisplay_delete();
  weatherdisplay_delete();
}


void show_watchface(void) {
  initialise_ui();
  window_set_background_color(s_window, GColorBlue);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  
  datedisplay_create(s_window);
  timedisplay_create(s_window);
  statusdisplay_create(s_window);
  weatherdisplay_create(s_window);
  
  // get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  update_time(tick_time);
  
  //BatteryChargeState battery = battery_state_service_peek();
  //statusdisplay_update_battery(battery);
  
  //bool connected = bluetooth_connection_service_peek();
  //statusdisplay_update_connection(connected);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  tick_timer_service_subscribe(SECOND_UNIT, DEBUGGY_second_handler);
  
  battery_state_service_subscribe(statusdisplay_update_battery);
  bluetooth_connection_service_subscribe(statusdisplay_update_connection);
  
  window_stack_push(s_window, true);
}

void hide_watchface(void) {
  window_stack_remove(s_window, true);
}

*/