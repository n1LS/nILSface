/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#include "draw.h"
#include "fonts.h"

// month drawing (16x7 bitmap) *************************************************

void draw_month(GContext *ctx, GPoint p, int month)
{
	uint16_t *ch = (uint16_t *)s_month_font[month];

	for (int dy = 0; dy < 7; dy++, ch++)
	{
		uint16_t pixels = *ch;

		for (int dx = 0; dx < 16; dx++, pixels <<= 1)
		{
			if (pixels & 0x8000)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}
		}
	}
}

// nubmer drawing **************************************************************

void draw_number(GContext *ctx, GPoint p, int num)
{
	uint16_t *ch = (uint16_t *)s_date_font[num];

	for (int dx = 0; dx < 8; dx++, ch++)
	{
		uint16_t pixels = *ch;

		for (int dy = 15; dy >= 0; dy--, pixels >>= 1)
		{
			if (pixels & 1)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}
		}
	}
}

void draw_number_outline(GContext *ctx, GPoint p, int num)
{
	// offset origin to match draw_number_position

	p.x -= 1;
	p.y += 1;

	uint16_t *ch = (uint16_t *)s_date_font_outline[num];

	for (int dx = 0; dx < 10; dx++, ch++)
	{
		uint16_t pixels = *ch;

		for (int dy = 15; dy >= 0; dy--, pixels >>= 1)
		{
			if (pixels & 1)
			{
				graphics_draw_pixel(ctx, GPoint(p.x + dx, p.y + dy));
			}
		}
	}
}

void draw_numbers(GContext *ctx, GPoint p, int num)
{
	draw_number(ctx, p, (num / 10) % 10);

	p.x += 10;

	draw_number(ctx, p, num % 10);
}

void draw_numbers_outline(GContext *ctx, GPoint p, int num, kColor color)
{
	GColor fg = (color == kBlackOnWhite) ? GColorBlack : GColorWhite;
	GColor bg = (color == kBlackOnWhite) ? GColorWhite : GColorBlack;

	// backgorund / outline

	graphics_context_set_stroke_color(ctx, bg);
	draw_number_outline(ctx, p, (num / 10) % 10);

	p.x += 10;

	draw_number_outline(ctx, p,  num % 10);

	p.x -= 10;

	// foreground

	graphics_context_set_stroke_color(ctx, fg);
	draw_numbers(ctx, p, num);
}
