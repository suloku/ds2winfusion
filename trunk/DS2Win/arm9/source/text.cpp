//Text needs to have the tilemap specified each time you use it, probably should have made
//it a class so that would not be necessary.

#include "text.h"

void printOut(u16* map, const char * str, int x, int y) {
	while(*str) {
		if(x==32) {x=0; y++;}
		if(y==24) return;
		map[y*32+x++]=(*str)-32;
		str++;
	}
}

void printOut(u16* map, const char * str, int x, int y, int n) {
	while(*str && 0<=--n) {
		if(x==32) {x=0; y++;}
		if(y==24) return;
		map[y*32+x++]=(*str)-32;
		str++;
	}
}

void loadText(u16 *map, u16 *palette) {
	dmaCopy((uint16 *)asciiPal, (uint16 *)palette, asciiPalLen);
	dmaCopy((uint16 *)asciiData, (uint16 *)map, asciiLen);
}

void printCenter(u16 *map, const char *str, u8 x1, u8 y1, u8 x2, u8 y2) {
	u8 x = x1+((x2 - x1 + 1 - strlen(str))/2);
	u8 y = y1+((y2 - y1)/2);
	printOut(map, str, x, y);
}

void clearText(u16 *map, u8 x1, u8 y1, u8 x2, u8 y2) {
	u8 x,y;
	for(y = y1; y <= y2; y++) {
		for(x = x1; x <= x2; x++)
			map[y*32+x]=0;
	}
}

