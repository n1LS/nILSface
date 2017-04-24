/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#ifndef __WEATHER_H
#define __WEATHER_H

#include <pebble.h>

#define NO_TEMPERATURE -32768

void weather_request();
void weather_init();
void weather_destroy();

GBitmap *weather_get_icon();
int weather_get_temperature();

#endif
