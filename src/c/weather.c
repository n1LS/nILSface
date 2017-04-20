/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include "weather.h"
#include "main.h"
#include "ui.h"

#include <pebble.h>

int s_temperature = NO_TEMPERATURE;

static GBitmap *s_current_icon;
static uint32_t s_current_icon_id = 0xffffffff;

static const uint32_t WEATHER_ICONS[] = 
{
	RESOURCE_ID_BMP_WEATHER_SUNNY,
	RESOURCE_ID_BMP_WEATHER_CLOUDY,
	RESOURCE_ID_BMP_WEATHER_RAINY,
	RESOURCE_ID_BMP_WEATHER_SNOWY
};

void weather_request()
{
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (!iter)
	{
		// Error creating outbound message
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Error creating outbound message");
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
	s_temperature = tuple->value->int32;

	// icon
	tuple = dict_read_next(iter);

	uint32_t icon = tuple->value->uint8;

	// free icon if we have one and it changed
	if (s_current_icon && s_current_icon_id != icon)
	{
		gbitmap_destroy(s_current_icon);
	}

	// load image if we don't already have one
	if (!s_current_icon)
	{
		s_current_icon = gbitmap_create_with_resource(WEATHER_ICONS[icon]);		
	}

	app_request_redraw();
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

	weather_request();
}
