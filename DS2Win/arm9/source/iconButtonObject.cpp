#include "iconButtonObject.h"

CIconButton::CIconButton(u16* map, u16* text, const IconButtonData* data, u16 ID) : CButton(map, text, (const GUIData*)data, ID) {
	mIcon = (u16) data->icon;
	mType = BUTTON;
	
	draw();
}


void CIconButton::draw() {
//	printCenter(mText, mTitle, x1, y1, x2, y2);
	setDown(mDown);

	setTile( x1+((x2-x1)/2) , y1+((y2-y1)/2) , mIcon);
}

void CIconButton::setDown(bool press){
	CButton::setDown(press);
	setTile( x1+((x2-x1)/2) , y1+((y2-y1)/2) , mIcon);	
}

