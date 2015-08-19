#pragma once

void weatherdisplay_create(Window *window);
void weatherdisplay_delete(void);

void weatherdisplay_update(struct tm *tick_time);

void HACK_draw_next_weather(void);