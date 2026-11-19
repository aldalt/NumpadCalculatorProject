// Display.h
// By Alexander Alt

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "fonts.h"
#include "main.h"

#define OLED_WIDTH         128
#define OLED_HEIGHT        64

#define OLED_SPI           hspi1

void ScreenWriteCommand(uint8_t Command);
void ScreenWriteData(uint8_t *Data, uint16_t Size);
void ScreenUpdate(void);
void ScreenInit(void);
void ScreenDrawChar(uint16_t x, uint16_t y, char Ch, FontType Font);
void ScreenDrawPixel(int16_t x, int16_t y, uint8_t Color);
void ScreenPrintString(uint16_t x, uint16_t y, const char *String, FontType Font);
void ScreenClear(void);
void PrintWithScroll(uint16_t x, uint16_t y, const char *String, FontType Font);

#endif
