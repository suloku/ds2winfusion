#include "labelObject.h"

CLabel::CLabel(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	mType = LABEL;

	draw();
}

void CLabel::draw() {
	printOut(mText, mTitle, x1, y1, x2-x1+1);
}

void CLabel::setText(char *title) {
	clearText(mText, x1, y1, x2, y2);
	strncpy(mTitle, title, 32);
	printOut(mText, mTitle, x1, y1, x2-x1+1);
}

void CLabel::setTextCenter(char *title) {
	clearText(mText, x1, y1, x2, y2);
	strncpy(mTitle, title, 32);
	printCenter(mText, mTitle, x1, y1, x2, y2);
}

