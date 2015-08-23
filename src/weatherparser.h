#pragma once
#include <pebble.h>

typedef struct weather
{
  // private bits
  char BACKING_STRING[50];
  
  //public bits
  char location_buffer[5];
  
  char *feels_like;
  char *precipitation_prob;
  char *description;
  int description_id;
  
  char *wind_speed;
  char *wind_gust;
  char *wind_direction_name;
  int wind_direction_pointer;
  
  int timestamp;
  //char *timestamp_hour;
  //char *timestamp_period;
  char timestamp_hour[3];
  char timestamp_period[3];
  
  bool is_valid;
} WeatherData;

void parse_weather(WeatherData *, char *);
void print_weather(WeatherData *);