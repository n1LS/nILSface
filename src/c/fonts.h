#ifndef __FONTS_H
#define __FONTS_H

static const uint16_t s_month_font[12][7] =
{
	{   // JAN
        0b0000100111010010,
        0b0000101001010010,
        0b0000101001010010,
        0b0000101001011010,
        0b0000101111010110,
        0b0100101001010010,
        0b0011001001010010,
	},
	{   // FEB
		0b0111101111011100,
		0b0100001000010010,
		0b0100001000010010,
		0b0111101110011100,
		0b0100001000010010,
		0b0100001000010010,
		0b0100001111011100,
	},
	{   // MAR
		0b0100010011001110,
		0b0110110100101001,
		0b0101010100101001,
		0b0100010111101110,
		0b0100010100101001,
		0b0100010100101001,
		0b0100010100101001,
	},
	{   // APR
		0b0011001110011100,
		0b0100101001010010,
		0b0100101001010010,
		0b0100101001010010,
		0b0111101110011100,
		0b0100101000010010,
		0b0100101000010010,
	},
	{   // MAY
		0b0100010011001001,
		0b0110110100101001,
		0b0101010100101001,
		0b0100010100101001,
		0b0100010111100111,
		0b0100010100100001,
		0b0100010100100001,
	},
	{   // JUN
		0b0000101001010010,
		0b0000101001010010,
		0b0000101001010010,
		0b0000101001011010,
		0b0000101001010110,
		0b0100101001010010,
		0b0011000110010010,
	},
	{   // JUL
		0b0000101001010000,
		0b0000101001010000,
		0b0000101001010000,
		0b0000101001010000,
		0b0000101001010000,
		0b0100101001010000,
		0b0011000110011110,
	},
	{   // AUG
		0b0011001001001100,
		0b0100101001010010,
		0b0100101001010000,
		0b0100101001010000,
		0b0111101001010110,
		0b0100101001010010,
		0b0100100110001110,
	},
	{   // SEP
		0b0011001111011100,
		0b0100101000010010,
		0b0100001000010010,
		0b0011001110010010,
		0b0000101000011100,
		0b0100101000010000,
		0b0011001111010000,
	},
	{   // OCT
		0b0011000110011111,
		0b0100101001000100,
		0b0100101000000100,
		0b0100101000000100,
		0b0100101000000100,
		0b0100101001000100,
		0b0011000110000100,
	},
	{   // NOV
		0b0100100110010010,
		0b0100101001010010,
		0b0100101001010010,
		0b0110101001010010,
		0b0101101001010100,
		0b0100101001011000,
		0b0100100110010000,
	},
	{   // DEC
		0b0111001111001100,
		0b0100101000010010,
		0b0100101000010000,
		0b0100101110010000,
		0b0100101000010000,
		0b0100101000010010,
		0b0111001111001100,
	},
};

// 13x8 font one word per column
static const uint16_t s_date_font[10][8] =
{
    { /* 0 */  0x03f8,  0x0ffe,  0x1e0f,  0x1803,  0x1803,  0x1e0f,  0x0ffe,  0x03f8 },
    { /* 1 */  0x0100,  0x0380,  0x0700,  0x0e00,  0x1fff,  0x1fff,  0x0000,  0x0000 },
    { /* 2 */  0x0607,  0x0e0f,  0x1c1f,  0x183b,  0x1873,  0x1ce3,  0x0fc3,  0x0783 },
    { /* 3 */  0x0404,  0x0e0e,  0x1c07,  0x1803,  0x18c3,  0x1de7,  0x0ffe,  0x073c },
    { /* 4 */  0x0038,  0x00f8,  0x03d8,  0x0f18,  0x1e18,  0x1fff,  0x1fff,  0x0018 },
    { /* 5 */  0x0004,  0x1f8e,  0x1f87,  0x1983,  0x1983,  0x19c7,  0x18fe,  0x187c },
    { /* 6 */  0x01fc,  0x03fe,  0x07c7,  0x0ec3,  0x1cc3,  0x18e7,  0x107e,  0x003c },
    { /* 7 */  0x1800,  0x1807,  0x181f,  0x187f,  0x19f0,  0x1fc0,  0x1f00,  0x1c00 },
    { /* 8 */  0x073c,  0x0ffe,  0x1de7,  0x18c3,  0x18c3,  0x1de7,  0x0ffe,  0x073c },
    { /* 9 */  0x0780,  0x0fc1,  0x1ce3,  0x1867,  0x186e,  0x1cfc,  0x0ff8,  0x07f0 }
};

static const uint16_t s_date_font_outline[10][10] = 
{
	{ /* 0 */ 0x0ff8, 0x380e, 0x6003, 0x43e1, 0x4e39, 0x4e39, 0x43e1, 0x6003, 0x380e, 0x0ff8, },
	{ /* 1 */ 0x0700, 0x0d80, 0x1880, 0x3180, 0x63ff, 0x4001, 0x4001, 0x7fff, 0x0000, 0x0000, },
	{ /* 2 */ 0x1e1f, 0x3231, 0x6261, 0x46c1, 0x4d89, 0x4f19, 0x4639, 0x6069, 0x30c9, 0x1f8f, },
	{ /* 3 */ 0x1c1c, 0x3636, 0x6223, 0x4631, 0x4fd9, 0x4e79, 0x4431, 0x6003, 0x3186, 0x1ffc, },
	{ /* 4 */ 0x00f8, 0x0388, 0x0e08, 0x3848, 0x61c8, 0x43cf, 0x4001, 0x4001, 0x7fcf, 0x0078, },
	{ /* 5 */ 0x001c, 0x7fb6, 0x40a3, 0x40b1, 0x4c99, 0x4cd9, 0x4c71, 0x4e03, 0x4b06, 0x79fc, },
	{ /* 6 */ 0x07fc, 0x0c06, 0x1803, 0x3071, 0x6259, 0x4679, 0x4e31, 0x5b03, 0x7186, 0x00fc, },
	{ /* 7 */ 0x7800, 0x481f, 0x4871, 0x49c1, 0x4f01, 0x4c1f, 0x4070, 0x41c0, 0x4700, 0x7c00, },
	{ /* 8 */ 0x1ffc, 0x3186, 0x6003, 0x4431, 0x4e79, 0x4e79, 0x4431, 0x6003, 0x3186, 0x1ffc, },
	{ /* 9 */ 0x1f80, 0x30c7, 0x606d, 0x4639, 0x4f31, 0x4f23, 0x4606, 0x600c, 0x3018, 0x1ff0, },
};

#endif
