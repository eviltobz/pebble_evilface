#pragma once

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
  
  char *timestamp_hour;
  char *timestamp_period;
  
  bool is_valid;
} WeatherData;

void parse_weather(WeatherData *, char *);
void print_weather(WeatherData *);