#pragma once

void timedisplay_create(Window *window);
void timedisplay_delete(void);

void timedisplay_update(struct tm *tick_time);