#pragma once
#include <pebble.h>

void weatherdisplay_create(Window *window);
void weatherdisplay_delete(void);

void weatherdisplay_update(struct tm *tick_time);
void weatherdisplay_reconnect();

void HACK_draw_next_weather(void);