#include <pebble.h>
#include "datedisplay.h"

static Window *s_window;
static GFont s_res_font_date;
static TextLayer *s_date;
static BitmapLayer *s_box;

static GFont s_res_font_caption;
static TextLayer *s_caption;

void datedisplay_create(Window *window) {
  s_window = window;
  s_res_font_caption = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_28));
  
  s_box = bitmap_layer_create(GRect(16, 122, 28, 28));
  bitmap_layer_set_background_color(s_box, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_box);
  
  s_date = text_layer_create(GRect(15, 118, 30, 35));
  text_layer_set_text(s_date, "  ");
  text_layer_set_background_color(s_date, GColorClear);
  text_layer_set_text_alignment(s_date, GTextAlignmentCenter);
  text_layer_set_font(s_date, s_res_font_date);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_date);
  
  s_caption = text_layer_create(GRect(0, 100, 60, 21));
  text_layer_set_background_color(s_caption, GColorClear);
  text_layer_set_text_color(s_caption, GColorWhite);
  text_layer_set_text_alignment(s_caption, GTextAlignmentCenter);
  text_layer_set_text(s_caption, "day, mon");
  text_layer_set_font(s_caption, s_res_font_caption);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_caption);
}

void datedisplay_delete(void) {
  text_layer_destroy(s_date);
  fonts_unload_custom_font(s_res_font_date);
  text_layer_destroy(s_caption);
  bitmap_layer_destroy(s_box);
}

void datedisplay_update(char *date, char *caption) {
  text_layer_set_text(s_date, date);
  text_layer_set_text(s_caption, caption);
}
