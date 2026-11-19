// Fonts.h
// By Alexander Alt

#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

typedef struct {
	const uint8_t *Data; // Byte arrays
	uint8_t Width; // Horizontal boundaries
	uint8_t Height; // Vertical pixel footprint
	uint8_t IsFixed16; // Tracks offset sizing (0 = 5x7, 1 = 8x12)
}FontType;

extern const uint8_t Bitmaps5x7[];
extern const uint8_t Bitmaps8x12[][16];

static const FontType SmallFont  = { Bitmaps5x7,  5, 7,  0 };
static const FontType MediumFont = { (const uint8_t*)Bitmaps8x12, 8, 12, 1 };

#endif
