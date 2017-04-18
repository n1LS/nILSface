/******************************************************************************
 nILSface (c) 2016-2017 nILS Podewski - nils.podewski.de - All rights reserved.
 ******************************************************************************/

#ifndef __DRAWING_H
#define __DRAWING_H

#include <pebble.h>

#include "drawing.h"
#include "fonts.h"

typedef enum kColor
{
	kBlackOnWhite,
	kWhiteOnBlack
} kColor;

void draw_month(GContext *ctx, GPoint p, int month);
void draw_number(GContext *ctx, GPoint p, int num);
void draw_number_outline(GContext *ctx, GPoint p, int num);
void draw_numbers(GContext *ctx, GPoint p, int num);
void draw_numbers_outline(GContext *ctx, GPoint p, int num, kColor color);

#endif
