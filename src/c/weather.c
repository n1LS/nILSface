/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include "weather.h"

#include <pebble.h>

static GBitmap *s_current_icon;

enum WeatherKey
{
	WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
	WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
};

static const uint32_t WEATHER_ICONS[] = {
	0,
	1,
	2,
	3
};

void weather_request()
{
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (!iter)
	{
		// Error creating outbound message
		return;
	}

	int value = 1;
	dict_write_int(iter, 1, &value, sizeof(int), true);
	dict_write_end(iter);

	app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
	static char buffer[10];

	// temperature

	Tuple *tuple = dict_read_first(iter);
	int32_t temperature = tuple->value->int32;
	snprintf(buffer, 10, "%d°", (int)temperature);
	// TODO: update info text_layer_set_text(s_city_layer, buffer);

	// icon
	tuple = dict_read_next(iter);

	int icon = tuple->value->uint8;

	if (s_current_icon)
	{
		gbitmap_destroy(s_current_icon);
	}

	s_current_icon = gbitmap_create_with_resource(WEATHER_ICONS[icon]);
	// bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
	// bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
}

void weather_init()
{
	app_message_register_inbox_received(inbox_received_callback);

	weather_request();
}
