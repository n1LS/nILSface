/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include "weather.h"
#include "main.h"
#include "ui.h"

#include <pebble.h>

int s_temperature = NO_TEMPERATURE;

static GBitmap *s_current_icon;
static int32_t s_current_icon_id = 0xffffffff;

#define s_number_of_icons 4

static const uint32_t s_weather_icons[s_number_of_icons] =
{
	RESOURCE_ID_BMP_WEATHER_SUNNY,
	RESOURCE_ID_BMP_WEATHER_CLOUDY,
	RESOURCE_ID_BMP_WEATHER_RAINY,
	RESOURCE_ID_BMP_WEATHER_SNOWY
};

void weather_request()
{
	DictionaryIterator *iter;
	AppMessageResult result = app_message_outbox_begin(&iter);

	if (!iter || result != APP_MSG_OK)
	{
		// Error creating outbound message
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Error creating outbound message [%d]", result);
		return;
	}

	int value = 1;
	dict_write_int(iter, 1, &value, sizeof(int), true);
	dict_write_end(iter);

	app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
	// temperature

	Tuple *tuple = dict_read_first(iter);

	if (!tuple)
	{
		APP_LOG(APP_LOG_LEVEL_DEBUG, "/!\\ Error reading 1st tuple -> crash?");
	}

	s_temperature = tuple->value->int32;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "(i) TEMPERATURE = %d", s_temperature);

	// icon
	tuple = dict_read_next(iter);

	if (!tuple)
	{
		APP_LOG(APP_LOG_LEVEL_DEBUG, "/!\\ Error reading 2nd tuple -> crash?");
	}

	int icon = tuple->value->int32;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "(i) ICON = %d", icon);

	// free icon if we have one and it changed
	if (s_current_icon && s_current_icon_id != icon)
	{
		gbitmap_destroy(s_current_icon);
		s_current_icon = NULL;
	}

	// load image if we don't already have one
	if (!s_current_icon && (icon >= 0) && (icon < s_number_of_icons))
	{
		APP_LOG(APP_LOG_LEVEL_DEBUG, "(i) Reloading the image");
		s_current_icon = gbitmap_create_with_resource(s_weather_icons[icon]);
	}

	app_request_redraw();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "(i) Done with the inbox callback");
}

GBitmap *weather_get_icon()
{
	return s_current_icon;
}

int weather_get_temperature()
{
	return s_temperature;
}

void weather_init()
{
	app_message_register_inbox_received(inbox_received_callback);
}

void weather_destroy()
{
	if (s_current_icon)
	{
		gbitmap_destroy(s_current_icon);
	}
}
