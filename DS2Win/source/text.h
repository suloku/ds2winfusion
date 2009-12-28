#pragma once

#include <string.h>
#include "nds.h"
#include "ascii.h"

void printOut(u16* map, const char * str, int x, int y);
void printOut(u16* map, const char * str, int x, int y, int n);
void loadText(u16 *map, u16 *palette);
void printCenter(u16 *map, const char *str, u8 x1, u8 y1, u8 x2, u8 y2);
void clearText(u16 *map, u8 x1, u8 y1, u8 x2, u8 y2);


