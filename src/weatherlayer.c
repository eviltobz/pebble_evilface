#include <pebble.h>
#include "weatherlayer.h"
#include "weatherparser.h"
#include "common.h"
#include "debugout.h"

Window *s_window;
  
// common across all calls
static int s_reference_count = 0;
static GFont *s_main_font;
static GFont *s_location_font;
static GFont *s_timestamp_font;
static GFont *s_small_font;

//static GBitmap *s_windicons;
static GFont *s_weather_icon_font;

// specific to an instance !! THIS COULD BE A POTENTIAL BUG !!
static bool initialising = true;

void draw_wind(WeatherLayer *layer, int direction) {
  int x = direction;
  int y = 0;
  if(direction > 5) {x = direction - 6; y=1;}
  if(direction > 11) {x = direction - 12; y=2;}
  GRect rect = GRect(x*40,y*40,40,40);
  gbitmap_set_bounds(layer->windicons, rect);
  bitmap_layer_set_bitmap(layer->winddirection, layer->windicons);
}

void draw_weather(WeatherLayer *layer, int type) {
  //if(s_reference_count == 0) return;  IS THIS EVEN RELEVANT NOW?!?!?
  // weather font from http://www.alessioatzeni.com/meteocons/ for more info.
  char *char_type = ")";
  switch(type) {
    case 0: char_type = "C"; break;
    case 1: char_type = "B"; break;
    case 2: char_type = "I"; break;
    case 3: char_type = "H"; break;
    case 4: char_type = ")"; break;
    case 5: 
    case 6: char_type = "M"; break;
    case 7:
    case 8: char_type = "N"; break;
    case 9:
    case 10:
    case 11:
    case 12: char_type = "Q"; break;
    case 13:
    case 14:
    case 15: char_type = "R"; break;
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21: char_type = "X"; break;
    case 22:
    case 23:
    case 24: char_type = "U"; break;
    case 25:
    case 26:
    case 27: char_type = "W"; break;
    case 28:
    case 29:
    case 30: char_type = "Z"; break;
  }
  text_layer_set_text(layer->summary_icon, char_type);
}

void HACK_weatherlayer_drawnext(WeatherLayer *layer) {
  LOG_DEBUG("CALLING HACK WEATHERLAYER DRAWNEXT!!!! - DOING NOTHING!!!!");
  /*
  static int HACK_weather_type = 0;
  draw_weather(HACK_weather_type);
  HACK_weather_type++;
  if(HACK_weather_type > 30) HACK_weather_type = 0;
  // skip things without their own icon...
  if(HACK_weather_type==5 || HACK_weather_type==7) HACK_weather_type++;
  if(HACK_weather_type>8 && HACK_weather_type<12) HACK_weather_type = 12;
  if(HACK_weather_type>12 && HACK_weather_type<15) HACK_weather_type = 15;
  if(HACK_weather_type>15 && HACK_weather_type<21) HACK_weather_type = 21;
  if(HACK_weather_type>21 && HACK_weather_type<24) HACK_weather_type = 24;
  if(HACK_weather_type>24 && HACK_weather_type<27) HACK_weather_type = 27;
  if(HACK_weather_type>27 && HACK_weather_type<30) HACK_weather_type = 30;
  */
  
  static int HACK_wind_dir = 0;
  draw_wind(layer, HACK_wind_dir);
  HACK_wind_dir++;
  if(HACK_wind_dir > 17) HACK_wind_dir = 0;
  
}

void weatherlayer_clear(WeatherLayer *layer) {
  text_layer_set_text(layer->windspeed, "");
  text_layer_set_text(layer->windgust, "");
  text_layer_set_text(layer->hour, "");
  text_layer_set_text(layer->period, "");

  text_layer_set_text(layer->temp_and_rain, "");

  text_layer_set_text(layer->summary, "");
  
  
  GRect rect = GRect(0,0,1,1);
  gbitmap_set_bounds(layer->windicons, rect);
  bitmap_layer_set_bitmap(layer->winddirection, layer->windicons);
  
  text_layer_set_text(layer->summary_icon, "");
  
  text_layer_set_text(layer->location, "");
}

void weatherlayer_update(WeatherLayer *layer, WeatherData *weather) {
  // temporarily not showing this...
  //FORMAT_STRING(layer->HACK_BUFFER, "%s %s", weather->timestamp_hour, weather->timestamp_period);
  //text_layer_set_text(layer->location, layer->HACK_BUFFER);
  text_layer_set_text(layer->location, weather->location_buffer);
  
  draw_wind(layer, weather->wind_direction_pointer);
  text_layer_set_text(layer->windspeed, weather->wind_speed); 
  text_layer_set_text(layer->windgust, weather->wind_gust);
  text_layer_set_text(layer->hour, weather->timestamp_hour);
  text_layer_set_text(layer->period, weather->timestamp_period);

  debugout_append(weather->timestamp_hour);
  debugout_append(weather->timestamp_period);
  debugout_append(",");
  
  FORMAT_STRING(layer->TEMP_AND_RAIN_BUFFER, "%s° %s", weather->feels_like, weather->precipitation_prob);
  text_layer_set_text(layer->temp_and_rain, layer->TEMP_AND_RAIN_BUFFER);
  draw_weather(layer, weather->description_id);

  FORMAT_STRING(layer->SUMMARY_BUFFER, "%s", weather->description);
  text_layer_set_text(layer->summary, layer->SUMMARY_BUFFER);
}


void create_wind_info(WeatherLayer **layer, int x, int y) {
  const int TEXT_OFFSET = 8;
  const int TSO = 12; // temp hack for spacing timestamp
  (*layer)->hour   = build_textlayer(GRect(x - 3, y+8,  20, 20), s_timestamp_font, GColorWhite, GTextAlignmentCenter);
  (*layer)->period = build_textlayer(GRect(x + 1, y+23, TSO, 20), s_small_font, GColorWhite, GTextAlignmentCenter);
  (*layer)->winddirection = build_bitmaplayer(GRect(x + 1 + TSO, y + 1, 40, 40), GColorClear);
  (*layer)->windspeed = build_default_textlayer(GRect(x+1+10 + TSO, y+TEXT_OFFSET, 20, 20), s_main_font);
  (*layer)->windgust  = build_textlayer(GRect(x+1+44 + TSO, y+TEXT_OFFSET, 20, 20), s_main_font, GColorWhite, GTextAlignmentLeft);
}

// LEAVE THIS AROUND TO DEMO * vs ** args in deved
// D'oh, why? does this work? what am i demoing??? - only worked with static layer???
void create_weather_infoX(WeatherLayer *layer, int x, int y) {
  LOG_DEBUG("create weather info X");
  layer->summary_icon = build_default_textlayer(GRect(x-2, y+2, 34, 34), s_weather_icon_font);
  layer->temp_and_rain = build_textlayer(GRect(x+29, y, 100, 20), s_main_font, GColorWhite, GTextAlignmentLeft);
  layer->summary = build_textlayer(GRect(x+29, y+20, 100, 15), s_small_font, GColorWhite, GTextAlignmentLeft);
}

void create_weather_info(WeatherLayer **layer, int x, int y) {
  (*layer)->summary_icon = build_default_textlayer(GRect(x-2, y+2, 34, 34), s_weather_icon_font);
  (*layer)->temp_and_rain = build_textlayer(GRect(x+29, y, 100, 20), s_main_font, GColorWhite, GTextAlignmentLeft);
  (*layer)->summary = build_textlayer(GRect(x+29, y+20, 100, 15), s_small_font, GColorWhite, GTextAlignmentLeft);
}


WeatherLayer* weatherlayer_create(Window *window, int y_offset) {
  s_window = window;
  
  if(s_reference_count == 0) {
    s_main_font      = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_20));
    s_timestamp_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_17));
    s_small_font     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_10));
    s_location_font  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_40));
    s_weather_icon_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_WEATHER_FONT_23));
  }
  
  WeatherLayer *layer = (WeatherLayer *) malloc(sizeof(WeatherLayer));
  layer->windicons = gbitmap_create_with_resource(RESOURCE_ID_WIND_DIRECTION_ICONS);
  //layer->location = build_textlayer(GRect(1, -5 + y_offset, 80, 70), s_location_font, GColorBlueMoon, GTextAlignmentLeft);
  layer->location = build_textlayer(GRect(1, 10 + y_offset, 80, 70), s_location_font, GColorBlueMoon, GTextAlignmentLeft);
    
  create_wind_info(&layer, 0, 0 + y_offset);
  create_weather_info(&layer, 0, 40 + y_offset);
  
  
  initialising = false;
  s_reference_count++;
  return layer;
}

void weatherlayer_delete(WeatherLayer *layer) {
  
  bitmap_layer_destroy(layer->winddirection);
  text_layer_destroy(layer->windspeed);
  text_layer_destroy(layer->windgust);
  text_layer_destroy(layer->hour);
  text_layer_destroy(layer->period);
  
  text_layer_destroy(layer->summary_icon);
  text_layer_destroy(layer->temp_and_rain);
  text_layer_destroy(layer->summary);
  text_layer_destroy(layer->location);
  
  gbitmap_destroy(layer->windicons);
  
  free(layer);
  s_reference_count--;
  
  if(s_reference_count == 0) {
    fonts_unload_custom_font(s_main_font);
    fonts_unload_custom_font(s_location_font);
    fonts_unload_custom_font(s_timestamp_font);
    fonts_unload_custom_font(s_small_font);
    fonts_unload_custom_font(s_weather_icon_font);
  }
}
