#pragma once
#include <pebble.h>
#include "weatherparser.h"
  


typedef struct weather_layer
{
  // private bits
  char TEMP_AND_RAIN_BUFFER[50];
  char SUMMARY_BUFFER[50];
  char HACK_BUFFER[50];
  // public bits
  // TextLayer *location;
  
  TextLayer *hour;
  TextLayer *period;
  
  BitmapLayer *winddirection;
  TextLayer *windspeed;
  TextLayer *windgust;
  
  TextLayer *summary_icon;
  TextLayer *summary;
  TextLayer *temp_and_rain;
  
  GBitmap *windicons;
  
} WeatherLayer;


WeatherLayer* weatherlayer_create(Window *window, int y_offset);
void weatherlayer_delete(WeatherLayer*);
void weatherlayer_clear(WeatherLayer*);

void weatherlayer_update(WeatherLayer*, WeatherData*);

void HACK_weatherlayer_drawnext(WeatherLayer*);