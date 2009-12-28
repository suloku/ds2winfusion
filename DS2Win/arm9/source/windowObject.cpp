//The base class for each window object.

#include "windowObject.h"

CWindowObject::CWindowObject(u16* map, u16* text, const GUIData* data, u16 ID) {
	mMap = map;
	mText = text;
	mID = ID;
	mState = 0;
	mType = OBJ_UNDEF;

	x1 = data->x1;
	y1 = data->y1;
	x2 = data->x2;
	y2 = data->y2;

	strncpy(mTitle, data->title, 32);
}


void CWindowObject::draw() {
	printCenter(mText, mTitle, x1, y1, x2, y2);
}

bool CWindowObject::touchInside() {
	u8 scrX1 = x1*8, scrY1 = y1*8, scrX2 = (x2+1)*8, scrY2 = (y2+1)*8;
	touchPosition touchXY=touchReadXY();
	
	lastX = touchXY.px;
	lastY = touchXY.py;

	if(touchXY.px > scrX1 && touchXY.px < scrX2 && touchXY.py > scrY1 && touchXY.py < scrY2)
		return true;

	return false;
}

bool CWindowObject::lastInside() {
	u8 scrX1 = x1*8, scrY1 = y1*8, scrX2 = (x2+1)*8, scrY2 = (y2+1)*8;
	
	if(lastX > scrX1 && lastX < scrX2 && lastY > scrY1 && lastY < scrY2)
		return true;

	return false;
}

void CWindowObject::setTile(u8 x, u8 y, u16 tile) {
	//Set the tile to use palette slot 3
	tile |= (3 << 12);
	mMap[TILE(x,y)] = tile;
}

void CWindowObject::setText(char *title) {
	clearText(mText, x1, y1, x2, y2);
	strncpy(mTitle, title, 32);
	printCenter(mText, mTitle, x1, y1, x2, y2);
}

void CWindowObject::setState(u8 state) {
	mState = state;
}

void CWindowObject::hide() {
	clearText(mText, x1, y1, x2, y2);
}

CWindowObject::~CWindowObject() {
	hide();
}

