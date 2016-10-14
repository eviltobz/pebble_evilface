#include <pebble.h>
#include <string.h>
#include "weatherparser.h"
#include "common.h"
#include "debugout.h"
  
  
void print_weather(WeatherData *weather) {
  LOG_INFO("WEATHER DATA:");
  LOGF_DEBUG("-feels like      : %s", weather->feels_like);
  LOGF_DEBUG("-wind speed      : %s", weather->wind_speed);
  LOGF_DEBUG("-wind gust       : %s", weather->wind_gust);
  LOGF_DEBUG("-wind dir1       : %s", weather->wind_direction_name);
  LOGF_DEBUG("-wind dir2       : %d", weather->wind_direction_pointer);
  LOGF_DEBUG("-precipitation   : %s", weather->precipitation_prob);
  LOGF_DEBUG("-description     : %s", weather->description);
  LOGF_DEBUG("-descriptionId   : %d", weather->description_id);
  LOGF_DEBUG("-timestamp hour  : %s", weather->timestamp_hour);
  LOGF_DEBUG("-timestamp period: %s", weather->timestamp_period);
}

static int s_current_index = 0;
static char* s_weather_string;

bool Reset(WeatherData * weather, char *weather_string) {
  if(strcmp("null", weather_string) == 0)
  {
    weather->is_valid = false;
  }
  else {
    s_current_index = 0;

    strncpy(weather->BACKING_STRING, weather_string, strlen(weather_string) + 1);
    s_weather_string = weather->BACKING_STRING;
    weather->is_valid = true;
  }
  return weather->is_valid;
}

char* ReadString(void) {
  int start = s_current_index;
  while((char)s_weather_string[++s_current_index] != (char)("\n"[0])){
    ;
  }
  s_weather_string[s_current_index++] = 0; // null terminate section of string & move on
  return &s_weather_string[start];
}

int ReadInt(void) {
  char *int_string;
  int_string = ReadString();
  return atoi(int_string);
}

void parse_timestamp(WeatherData *weather) {
    int hour = weather->timestamp;
  
    if(hour < 12)
      strcpy(weather->timestamp_period, "am");
    else
      strcpy(weather->timestamp_period, "pm");
    if(hour > 12)
      hour -= 12;
    FORMAT_STRING(weather->timestamp_hour, "%d", hour);
    
    
}

void parse_weather(WeatherData *weather, char *weather_string) {
  if(Reset(weather, weather_string)) {
    weather->feels_like = ReadString();
    weather->wind_speed = ReadString();
    weather->wind_gust = ReadString();
    weather->wind_direction_name = ReadString();
    weather->wind_direction_pointer = ReadInt();
    weather->precipitation_prob = ReadString();
    weather->description = ReadString();
    weather->description_id = ReadInt();
    //int hour = ReadInt();
    /*
    if(weather->timestamp < 12)
      strcpy(weather->timestamp_period, "am");
    else
      strcpy(weather->timestamp_period, "pm");
    if(weather->timestamp > 12)
      hour -= 12;
    FORMAT_STRING(weather->timestamp_hour, "%d", hour);
    */
    weather->timestamp = ReadInt();
    parse_timestamp(weather);
    
    strcpy(weather->location_buffer, ReadString());
  };
}
