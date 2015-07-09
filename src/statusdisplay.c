#include <pebble.h>
#include "statusdisplay.h"

static GFont s_res_font;
static TextLayer *s_status;
static char charge_buffer[] = "Charging...";
static bool allow_vibes = false;

void statusdisplay_create(Window *window) {
  s_res_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  //s_res_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_status = text_layer_create(GRect(0, 147, 60, 21));
  //s_status = text_layer_create(GRect(0, 151, 60, 21));
  text_layer_set_background_color(s_status, GColorClear);
  text_layer_set_text_color(s_status, GColorWhite);
  text_layer_set_text(s_status, "X%");
  text_layer_set_text_alignment(s_status, GTextAlignmentCenter);
  text_layer_set_font(s_status, s_res_font);
  layer_add_child(window_get_root_layer(window), (Layer *)s_status);
  
  
  BatteryChargeState battery = battery_state_service_peek();
  statusdisplay_update_battery(battery);
  
  bool connected = bluetooth_connection_service_peek();
  statusdisplay_update_connection(connected);
  // vibes can be annoying if you're on the cusp of bluetooth range.
  // mebe fire a timer when status changes, and only vibe after a minute if 
  // the status hasn't changed again in the interim.
  //allow_vibes = true;
}

void statusdisplay_delete(void) {
  text_layer_destroy(s_status);
}

void statusdisplay_update_battery(BatteryChargeState state) {
  if(state.is_charging)
    snprintf(charge_buffer, sizeof(charge_buffer), "Charging");
  else 
    snprintf(charge_buffer, sizeof(charge_buffer), "%d%%", state.charge_percent);
  
  text_layer_set_text(s_status, charge_buffer);
}

void statusdisplay_update_connection(bool connected) {
  GColor status_colour = GColorWhite;
  if(connected){
    if(allow_vibes) vibes_short_pulse();
  }
  else {
    if(allow_vibes) vibes_double_pulse();
    status_colour = GColorRed;
  }
  
  text_layer_set_text_color(s_status, status_colour);
}