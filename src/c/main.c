#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>

#include "fonts.h"

static char time_hours[3] = "11";
static char time_minutes[3] = "00";

Layer* clock_area_layer;
FFont* hours_font;
FFont* minutes_font;

static const int s_side_width = 30;

#define NO_TEMPERATURE -32768

static GBitmap *s_bmp_battery;
static GBitmap *s_bmp_temperature;
static GBitmap *s_bmp_bluetooth;
static GBitmap *s_bmp_calendar;

static Window *s_main_window;
static Layer *s_canvas_layer;

static BatteryChargeState s_battery;
static int s_temperature = NO_TEMPERATURE;
static int s_day;
static int s_month;
static bool s_no_bluetooth;

// month drawing (16x7 bitmap) *************************************************

static void draw_month(GContext *ctx, GPoint p, int month)
{
	uint16_t *ch = (uint16_t *)s_month_font[month];

	for (int dy = 0; dy < 7; dy++)
	{
		uint16_t pixels = *ch;

		for (int dx = 0; dx < 16; dx++)
		{
			if (pixels & 0x8000)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}

			pixels <<= 1;
		}

		ch++;
	}

}

// day drawing *****************************************************************

static void draw_number(GContext *ctx, GPoint p, int num)
{
	uint16_t *ch = (uint16_t *)s_date_font[num];

	for (int dx = 0; dx < 8; dx++)
	{
		uint16_t pixels = *ch;

		for (int dy = 15; dy >= 0; dy--)
		{
			if (pixels & 1)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}

			pixels >>= 1;
		}

		ch++;
	}
}

static void draw_number_outline(GContext *ctx, GPoint p, int num)
{
    // offset origin to match draw_number_position
    p.x -= 1;
    p.y -= 1;
    
	uint16_t *ch = (uint16_t *)s_date_font_outline[num];

	for (int dx = 0; dx < 10; dx++)
	{
		uint16_t pixels = *ch;

		for (int dy = 14; dy >= 0; dy--)
		{
			if (pixels & 1)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}

			pixels >>= 1;
		}

		ch++;
	}
}

static void draw_numbers(GContext *ctx, GPoint p, int num)
{
    graphics_context_set_stroke_color(ctx, GColorWhite);
	draw_number_outline(ctx, p, (num / 10) % 10);
    graphics_context_set_stroke_color(ctx, GColorBlack);
//	draw_number(ctx, p, (num / 10) % 10);

    p.x += 10;

    graphics_context_set_stroke_color(ctx, GColorWhite);
    draw_number_outline(ctx, p,  num % 10);
    graphics_context_set_stroke_color(ctx, GColorBlack);
//	draw_number(ctx, p, num % 10);
}

// *********************************** UI **************************************

static void prv_tick_handler(struct tm *tick_time, TimeUnits changed)
{
	// store time
	int h = tick_time->tm_hour;
    time_hours[0] = '0' + h / 10;
    time_hours[1] = '0' + h % 10;
    
	int m = tick_time->tm_min;
    time_minutes[0] = '0' + m / 10;
    time_minutes[1] = '0' + m % 10;

	char buffer[3];
    strftime(buffer, sizeof(buffer), "%e", tick_time);
	s_day = atoi(buffer);

	strftime(buffer, sizeof(buffer), "%m", tick_time);
	s_month = atoi(buffer) - 1;

	// redraw
	if (s_canvas_layer)
	{
		layer_mark_dirty(s_canvas_layer);
	}
}

static void draw_calendar(GContext *ctx, GRect bounds)
{
	GRect r;

    // background image
    int height = 30;
    
    r.origin = GPoint(bounds.size.w - s_side_width, bounds.size.h - 2 - height);
    r.size = GSize(s_side_width, height);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_bmp_calendar, r);

    // day number
    GPoint p;
    p.x = bounds.size.w - s_side_width + 6;
    p.y = bounds.size.h - 22;
	draw_numbers(ctx, p, s_day);
    draw_numbers(ctx, GPoint(1, 0), s_day);

	// month name
	graphics_context_set_stroke_color(ctx, GColorWhite);
    p.x = bounds.size.w - (4 * 3 + 8);
    p.y = bounds.size.h - 29;
	draw_month(ctx, p, s_month % 12);
}

static void draw_bluetooth_icon(GContext *ctx, GRect bounds)
{
	GRect r;

	r.origin.x = bounds.size.w - s_side_width;
	r.origin.y = bounds.size.h - 71; // TODO CALENDAR HEIGHT
	r.size = GSize(30, 26);

	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, s_bmp_bluetooth, r);
}

static void draw_temperature(GContext *ctx, GRect bounds)
{
    if (s_temperature == NO_TEMPERATURE)
    {
        return;
    }
 
   	GRect r;

    int yOffs = 97; // TODO useful alignment method
    
    // background image
    r.origin = GPoint(bounds.size.w - s_side_width, bounds.size.h - yOffs);
    r.size = GSize(30, 15);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_bmp_temperature, r);

    GPoint p;
    p.x = bounds.size.w - s_side_width + 4;
    p.y = bounds.size.h - yOffs - 2; 
    
    graphics_context_set_stroke_color(ctx, GColorBlack);
    draw_numbers(ctx, p, s_temperature);
}

static void draw_battery(GContext *ctx, GRect bounds)
{
	GRect r;

    // background image
    r.origin = GPoint(bounds.size.w - s_side_width + 2, 2);
    r.size = GSize(26, 15);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_bmp_battery, r);
    
    if (s_battery.is_plugged || s_battery.is_charging)
    {
        return;    
    }
    
    // filled battery percentage
    r.origin.y = 6;
    r.origin.x = bounds.size.w - s_side_width + 6;
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
}

static void draw_time(GContext *ctx, GRect bounds)
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
    fctx_set_text_em_height(&fctx, hours_font, font_size);
    fctx_set_text_em_height(&fctx, minutes_font, font_size);

    // draw hours
    time_pos.x = INT_TO_FIXED(bounds.size.w / 2 + h_adjust);
    time_pos.y = INT_TO_FIXED(v_padding + v_adjust);
    fctx_set_offset(&fctx, time_pos);
    fctx_draw_string(&fctx, time_hours, hours_font, GTextAlignmentCenter, FTextAnchorTop);

    // draw minutes
    time_pos.y = INT_TO_FIXED(bounds.size.h - v_padding + v_adjust);
    fctx_set_offset(&fctx, time_pos);
    fctx_draw_string(&fctx, time_minutes, minutes_font, GTextAlignmentCenter, FTextAnchorBaseline);
    fctx_end_fill(&fctx);

    fctx_deinit_context(&fctx);
}

static void prv_update_proc(Layer *layer, GContext *ctx)
{
	const GRect bounds = layer_get_unobstructed_bounds(layer);

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

	// battery
	draw_battery(ctx, bounds);

	// calendar
	draw_calendar(ctx, bounds);

    // weather
    draw_temperature(ctx, bounds);
    
	// bluetooth status (if disconnected)
	if (s_no_bluetooth)
	{
		draw_bluetooth_icon(ctx, bounds);	
	}
}

static void prv_create_canvas() {
	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_unobstructed_bounds(window_layer);

	s_canvas_layer = layer_create(bounds);
	layer_set_update_proc(s_canvas_layer, prv_update_proc);
	layer_add_child(window_layer, s_canvas_layer);
}

/*********************************** App **************************************/

// Event fires once, before the obstruction appears or disappears
static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
	layer_mark_dirty(s_canvas_layer);
}

// Event fires once, after obstruction appears or disappears
static void prv_unobstructed_did_change(void *context) {
	layer_mark_dirty(s_canvas_layer);
}

static void prv_window_load(Window *window) {
	prv_create_canvas();

	tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);

    // load resources
    s_bmp_battery     = gbitmap_create_with_resource(RESOURCE_ID_BMP_BATTERY);
    s_bmp_temperature = gbitmap_create_with_resource(RESOURCE_ID_BMP_TEMPERATURE);
    s_bmp_bluetooth   = gbitmap_create_with_resource(RESOURCE_ID_BMP_NO_BLUETOOTH);
    s_bmp_calendar    = gbitmap_create_with_resource(RESOURCE_ID_BMP_CALENDAR);

	// Subscribe to the unobstructed area events
	UnobstructedAreaHandlers handlers = {
		.will_change = prv_unobstructed_will_change,
		.did_change = prv_unobstructed_did_change
	};

	unobstructed_area_service_subscribe(handlers, NULL);
}

void bluetooth_handler(bool connected)
{
	s_no_bluetooth = !connected;
}

void battery_handler(BatteryChargeState charge)
{
   	// store battery life
	s_battery = battery_state_service_peek();

    layer_mark_dirty(s_canvas_layer);
}

static void prv_window_unload(Window *window) {
	layer_destroy(s_canvas_layer);
    
    gbitmap_destroy(s_bmp_battery);
    gbitmap_destroy(s_bmp_bluetooth);
    gbitmap_destroy(s_bmp_temperature);
    gbitmap_destroy(s_bmp_calendar);
    
    ffont_destroy(minutes_font);
    ffont_destroy(hours_font);
}

static void prv_init() {
	time_t t = time(NULL);
	struct tm *time_now = localtime(&t);
	
    prv_tick_handler(time_now, MINUTE_UNIT);

	s_battery = battery_state_service_peek();
    battery_state_service_subscribe(battery_handler);

    s_no_bluetooth = !connection_service_peek_pebble_app_connection();
    bluetooth_connection_service_subscribe(bluetooth_handler);

    minutes_font = ffont_create_from_resource(RESOURCE_ID_FONT_NUMBERS_BOLD);
    hours_font = ffont_create_from_resource(RESOURCE_ID_FONT_NUMBERS);

	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = prv_window_load,
		.unload = prv_window_unload,
	});

	window_stack_push(s_main_window, true);
}

static void prv_deinit() 
{
	window_destroy(s_main_window);
    
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
}

int main() 
{
	prv_init();
	app_event_loop();
	prv_deinit();
}
