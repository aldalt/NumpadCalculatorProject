// Keypad.h
// By Alexander Alt

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include "main.h"
#include <stdint.h>

#define NUMROWS 5
#define NUMCOLS 6

char ScanKeyPress(void);
char GetCurrentKey(void);

#endif
