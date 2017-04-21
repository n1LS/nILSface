/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include <pebble.h>

#include "main.h"

#include "ui.h"
#include "weather.h"

static Window *s_main_window;
static Layer *s_canvas_layer;
static int weather_refresh_timer = 0;

// event handler ***************************************************************

// minute_unit tick handler
static void tick_handler(struct tm *tick_time, TimeUnits changed)
{
	// store time
	ui_set_time(tick_time);
	
	// call for a redraw
	if (s_canvas_layer)
	{
		app_request_redraw();
	}

	// periodically check the weather
	if (--weather_refresh_timer <= 0) 
	{
		weather_refresh_timer = 15;
		weather_request();
	}
}

static void update_procedure(Layer *layer, GContext *ctx)
{
	ui_redraw(ctx, layer_get_unobstructed_bounds(layer));
}

// event fires once, before the obstruction appears or disappears
static void unobstructed_change(GRect final_unobstructed_screen_area, void *context)
{
	app_request_redraw();
}

// event fires once, after obstruction appears or disappears
static void unobstructed_changed(void *context)
{
	app_request_redraw();
}

void bluetooth_handler(bool connected)
{
	// store connection status
	ui_set_bluetooth(connected);

	app_request_redraw();
}

void battery_handler(BatteryChargeState charge)
{
	// store battery life
	ui_set_battery(charge);

	app_request_redraw();
}

// App *************************************************************************

static void create_canvas()
{
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_unobstructed_bounds(window_layer);

	s_canvas_layer = layer_create(bounds);
	layer_set_update_proc(s_canvas_layer, update_procedure);
	layer_add_child(window_layer, s_canvas_layer);
}

static void window_load(Window *window)
{
	create_canvas();

	ui_init();

	// subscribe to the unobstructed area events
	UnobstructedAreaHandlers handlers = {
		.will_change = unobstructed_change,
		.did_change = unobstructed_changed
	};

	unobstructed_area_service_subscribe(handlers, NULL);

	// subscribe to time service
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void window_unload(Window *window)
{
	layer_destroy(s_canvas_layer);

	unobstructed_area_service_unsubscribe();

	ui_destroy();
}

static void init()
{
	const time_t t = time(NULL);
	struct tm *time_now = localtime(&t);
	
    battery_state_service_subscribe(battery_handler);
    bluetooth_connection_service_subscribe(bluetooth_handler);

	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	window_stack_push(s_main_window, true);

	tick_handler(time_now, MINUTE_UNIT | HOUR_UNIT);

	app_message_open(64, 64);

	weather_init();
}

static void deinit() 
{
	window_destroy(s_main_window);
    
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
}

void app_request_redraw() 
{
	layer_mark_dirty(s_canvas_layer);
}

// main ************************************************************************

int main() 
{
	init();
	app_event_loop();
	deinit();
}
