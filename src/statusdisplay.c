#include <pebble.h>
#include "statusdisplay.h"
#include "common.h"

static Layer *s_status_layer;
static bool s_allow_vibes = false;
static bool s_bluetooth_connected = true;
static GColor s_border_colour;
static GColor s_charged_colour;
static GColor s_empty_colour;

void statusdisplay_update_battery(BatteryChargeState state) {
  if(state.is_charging) {
    s_charged_colour = GColorYellow;
    s_empty_colour = GColorIndigo;
  }
  else {
    s_charged_colour = GColorIslamicGreen;
    s_empty_colour = GColorDarkCandyAppleRed;
  }
  layer_mark_dirty(s_status_layer);
}

void statusdisplay_update_connection(bool connected) {
  s_bluetooth_connected = connected;
  if(connected){
    if(s_allow_vibes) vibes_short_pulse();
    s_border_colour = GColorWhite;
  }
  else {
    if(s_allow_vibes) vibes_double_pulse();
    s_border_colour = GColorRed;
  }
  layer_mark_dirty(s_status_layer);
}

static void battery_layer_draw(Layer *layer, GContext *ctx) {
  // outline
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_stroke_color(ctx, s_border_colour);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_draw_rect(ctx, bounds);
  int width = bounds.size.w;
  int height = bounds.size.h;
    
  int battery_level = battery_state_service_peek().charge_percent / 10;
  bounds.origin.y = 1;
  bounds.size.w = 4;
  bounds.size.h = bounds.size.h - 2;
  
  for(int cell = 0; cell < 10; cell++) {
    //bounds.origin.x = (cell * 6) + 2;
    bounds.origin.x = (cell * 5) + 1;
    if(cell+1 <= battery_level)
      graphics_context_set_fill_color(ctx, s_charged_colour);
    else
      graphics_context_set_fill_color(ctx, s_empty_colour);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  }
  
  if(!s_bluetooth_connected) {
    bounds.origin.x = 1;
    bounds.origin.y = 1;
    bounds.size.w = width - 2;
    bounds.size.h = height - 2;
    graphics_draw_rect(ctx, bounds);
  }
}

void statusdisplay_create(Window *window) {
  //s_status_layer = layer_create(GRect(79, 156, 62, 10));
  s_status_layer = layer_create(GRect(90, 156, 51, 10));
  layer_add_child(window_get_root_layer(window), s_status_layer);
  layer_set_update_proc(s_status_layer, battery_layer_draw);
  
  BatteryChargeState battery = battery_state_service_peek();
  statusdisplay_update_battery(battery);
  
  bool connected = bluetooth_connection_service_peek();
  statusdisplay_update_connection(connected);
  
  battery_state_service_subscribe(statusdisplay_update_battery);
  bluetooth_connection_service_subscribe(statusdisplay_update_connection);
  
  // vibes can be annoying if you're on the cusp of bluetooth range.
  // mebe fire a timer when status changes, and only vibe after a minute if 
  // the status hasn't changed again in the interim.
  // s_allow_vibes = true;
}

void statusdisplay_delete(void) {
  layer_destroy(s_status_layer);
}