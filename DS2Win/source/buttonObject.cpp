#include "buttonObject.h"

CButton::CButton(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	mPressed = false;
	mDown = false;
	mState = BTN_UP;
	mType = BUTTON;

	draw();
}

void CButton::draw() {
	printCenter(mText, mTitle, x1, y1, x2, y2);
	setDown(mDown);
}

void CButton::processInput(u8 input) {
	switch (input) {
		case TOUCH_DOWN:
			if (touchInside()) {
				setDown(true);
				mPressed = true;
			} break;
		case TOUCH_HELD:
			if (mPressed && !mDown && touchInside())
				setDown(true);
			else if (mPressed && mDown && !touchInside())
				setDown(false);
			break;
		case TOUCH_UP:
			if (mDown)
				setDown(false);
			if (mPressed && lastInside())
				mState = BTN_ACTIVATED;

			mPressed = false;
			break;
	}
}

void CButton::setDown(bool press) {
	u16 tile = 0;
	u8 x,y;
	
	mDown = press;

	if (press) {
		mState = BTN_DOWN;
	} else
		mState = BTN_UP;

	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++) {
			if (x == x1) {
				if (y == y1)
					tile = BUTTON_TOP_LEFT;
				else if (y == y2)
					tile = BUTTON_BOTTOM_LEFT;
				else
					tile = BUTTON_LEFT;
			} else if (x == x2) {
				if (y == y1)
					tile = BUTTON_TOP_RIGHT;
				else if (y == y2)
					tile = BUTTON_BOTTOM_RIGHT;
				else
					tile = BUTTON_RIGHT;
			} else {
				if (y == y1)
					tile = BUTTON_TOP;
				else if (y == y2)
					tile = BUTTON_BOTTOM;
				else
					tile = BUTTON_BLANK;
			}

			if (mDown)
				tile+=9;

			setTile(x,y,tile);
		}
	}
}

