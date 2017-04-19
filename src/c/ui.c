/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include "ui.h"
#include "draw.h"
#include "weather.h"

#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

// constants *******************************************************************

static const int s_side_width = 30;

// static variables ************************************************************

static FFont *s_font;
static FFont *s_font_bold;

static GBitmap *s_bmp_battery;
static GBitmap *s_bmp_temperature;
static GBitmap *s_bmp_bluetooth;
static GBitmap *s_bmp_calendar;

static char time_hours[3] = "11";
static char time_minutes[3] = "00";

static BatteryChargeState s_battery;
static int s_day;
static int s_month;
static bool s_bluetooth;

// implementation **************************************************************

void ui_init()
{
	// load resources
	s_bmp_battery     = gbitmap_create_with_resource(RESOURCE_ID_BMP_BATTERY);
	s_bmp_temperature = gbitmap_create_with_resource(RESOURCE_ID_BMP_TEMPERATURE);
	s_bmp_bluetooth   = gbitmap_create_with_resource(RESOURCE_ID_BMP_NO_BLUETOOTH);
	s_bmp_calendar    = gbitmap_create_with_resource(RESOURCE_ID_BMP_CALENDAR);

	s_font_bold = ffont_create_from_resource(RESOURCE_ID_FONT_NUMBERS_BOLD);
	s_font = ffont_create_from_resource(RESOURCE_ID_FONT_NUMBERS);

	// init values
	s_battery = battery_state_service_peek();
	s_bluetooth = connection_service_peek_pebble_app_connection();
}

void ui_destroy()
{
	gbitmap_destroy(s_bmp_battery);
	gbitmap_destroy(s_bmp_bluetooth);
	gbitmap_destroy(s_bmp_temperature);
	gbitmap_destroy(s_bmp_calendar);

	ffont_destroy(s_font_bold);
	ffont_destroy(s_font);
}

void ui_set_bluetooth(bool connected)
{
	s_bluetooth = connected;
}

void ui_set_battery(BatteryChargeState state)
{
	s_battery = state;
}

void ui_set_time(struct tm *tick_time)
{
	int h = tick_time->tm_hour;

	time_hours[0] = '0' + h / 10;
	time_hours[1] = '0' + h % 10;

	int m = tick_time->tm_min;
	time_minutes[0] = '0' + m / 10;
	time_minutes[1] = '0' + m % 10;

	// store date
	char buffer[3];
	strftime(buffer, sizeof(buffer), "%e", tick_time);
	s_day = atoi(buffer);

	strftime(buffer, sizeof(buffer), "%m", tick_time);
	s_month = atoi(buffer) - 1;
}


int draw_calendar(GContext *ctx, GRect bounds)
{
	GRect r;

	// background image
	const int height = 30;

	r.origin = GPoint(bounds.origin.x, bounds.origin.y + bounds.size.h - 2 - height);
	r.size = GSize(s_side_width, height);
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bmp_calendar, r);

	// day number
	GPoint p;
	p.x = bounds.origin.x + 6;
	p.y = bounds.origin.y + bounds.size.h - height + 8;
	draw_numbers(ctx, p, s_day);

	// month name
	graphics_context_set_stroke_color(ctx, GColorWhite);
	p.x = bounds.origin.x + 7;
	p.y = bounds.origin.y + bounds.size.h - height + 1;
	draw_month(ctx, p, s_month % 12);

	return height + 2;
}

int draw_bluetooth_icon(GContext *ctx, GRect bounds)
{
	if (s_bluetooth)
	{
		// do not draw if we have a connection
		return 0;
	}

	const int height = 27;

	GRect r;

	r.origin.x = bounds.origin.x;
	r.origin.y = bounds.origin.y + bounds.size.h - 2 - height;
	r.size = GSize(s_side_width, height);

	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bmp_bluetooth, r);

	return height + 2;
}

int draw_temperature(GContext *ctx, GRect bounds)
{
	if (weather_temperature == NO_TEMPERATURE)
	{
		return 0;
	}

	int height = 17;

	// background image
	/*
	 GRect r;

	 r.origin = GPoint(bounds.size.w - s_side_width, bounds.size.h - yOffs);
	 r.size = GSize(30, 15);
	 graphics_context_set_compositing_mode(ctx, GCompOpSet);
	 graphics_draw_bitmap_in_rect(ctx, s_bmp_temperature, r);
	 */

	GPoint p;
	p.x = bounds.origin.x + 5;
	p.y = bounds.origin.y + bounds.size.h - height - 2;

	graphics_context_set_stroke_color(ctx, GColorBlack);
	draw_numbers_outline(ctx, p, weather_temperature, kBlackOnWhite);

	return height + 2;
}

int draw_battery(GContext *ctx, GRect bounds)
{
	GRect r;

	// background image
	r.origin = GPoint(bounds.origin.x + 2, 2);
	r.size = GSize(26, 15);
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bmp_battery, r);

	if (s_battery.is_plugged || s_battery.is_charging)
	{
		// do not draw fill percentage when charging or plugged in
		return 15;
	}

	// filled battery percentage
	r.origin.y = 6;
	r.origin.x = bounds.origin.x + 6;
	r.size.h = 7;
	r.size.w = s_side_width - 14;
	r.size.w = (r.size.w * s_battery.charge_percent) / 100;

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, r, 0, GCornerNone);

	// clear rest
	r.origin.x += r.size.w;
	r.size.w = (s_side_width - 14) - r.size.w;
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, r, 0, GCornerNone);

	return 15;
}

void draw_time(GContext *ctx, GRect bounds)
{
	// initialize FCTX, the fancy 3rd party drawing library that all the cool kids use
	FContext fctx;

	bounds.size.w -= s_side_width;

	fctx_init_context(&fctx, ctx);
	fctx_set_color_bias(&fctx, 0);
	fctx_set_fill_color(&fctx, GColorWhite);

	// calculate font size
	int font_size = 4 * bounds.size.h / 7;

	// font/font bold metrics
	int v_padding = bounds.size.h / 16;
	int h_adjust = 0;
	int v_adjust = 0;

	FPoint time_pos;
	fctx_begin_fill(&fctx);
	fctx_set_text_em_height(&fctx, s_font, font_size);
	fctx_set_text_em_height(&fctx, s_font_bold, font_size);

	// draw hours
	time_pos.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust);
	time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
	fctx_set_offset(&fctx, time_pos);
	fctx_draw_string(&fctx, time_hours, s_font, GTextAlignmentCenter, FTextAnchorTop);

	// draw minutes
	time_pos.y = INT_TO_FIXED(bounds.size.h - v_padding + v_adjust);
	fctx_set_offset(&fctx, time_pos);
	fctx_draw_string(&fctx, time_minutes, s_font_bold, GTextAlignmentCenter, FTextAnchorBaseline);
	fctx_end_fill(&fctx);

	fctx_deinit_context(&fctx);
}

void draw_side_bar(GContext *ctx, GRect bounds)
{
	int height;

	// aligned top:

	// battery
	height = draw_battery(ctx, bounds);
	bounds.origin.y += height;
	bounds.size.h -= height;

	// aligned bottom:

	// calendar
	height = draw_calendar(ctx, bounds);
	bounds.size.h -= height;

	// bluetooth status (if disconnected)
	height = draw_bluetooth_icon(ctx, bounds);
	bounds.size.h -= height;

	// weather
	height = draw_temperature(ctx, bounds);
	bounds.size.h -= height;
}

void ui_redraw(GContext *ctx, GRect bounds)
{
	GRect r;

	// fill main area
	r = bounds;
	r.size.w -= s_side_width;
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, r, 0, GCornerNone);

	// fill right border
	r.origin.x = r.size.w;
	r.size.w = s_side_width;
	graphics_context_set_fill_color(ctx, GColorDarkGray);
	graphics_fill_rect(ctx, r, 0, GCornerNone);

	// time
	draw_time(ctx, bounds);

	// side bar
	r = bounds;
	r.origin.x = r.size.w - s_side_width;
	r.size.w = s_side_width;
	draw_side_bar(ctx, r);
}
