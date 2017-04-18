/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#ifndef __UI_H
#define __UI_H

#include <pebble.h>

#define NO_TEMPERATURE -32768

extern int s_temperature;

void ui_redraw(GContext *ctx, GRect bounds);

void ui_set_time(struct tm *tick_time);
void ui_set_battery(BatteryChargeState state);
void ui_set_bluetooth(bool connected);

void ui_init();
void ui_destroy();

#endif
