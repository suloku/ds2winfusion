#include "radioButtonObject.h"

CRadioButton::CRadioButton(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	mPressed = false;
	mSelected = false;
	mState = RDO_UNSELECTED;
	mType = RADIO;

	draw();
}

void CRadioButton::draw() {
	printOut(mText, mTitle, x1+2, y1, x2-x1-1);
	setSelected(mSelected);	
}

void CRadioButton::processInput(u8 input) {
	switch (input) {
		case TOUCH_DOWN:
			if (touchInside()) {
				mPressed = true;
				setTile(x1,y1,RADIO_PRESSED);
			} break;
		case TOUCH_HELD:
			if (mPressed && touchInside())
				setTile(x1,y1,RADIO_PRESSED);
			else if (mPressed && !touchInside())
				setSelected(mSelected);
			break;
		case TOUCH_UP:
			if (mPressed && lastInside())
				setSelected(true);

			mPressed = false;
			break;
	}
}

void CRadioButton::setSelected(bool sel) {
	mSelected = sel;
	if (mSelected) {
		mState = RDO_SELECTED;
		setTile(x1,y1,RADIO_SELECTED);
	} else {
		mState = RDO_UNSELECTED;
		setTile(x1,y1,RADIO_UNSELECTED);
	}
}

void CRadioButton::setState(u8 state) {
	switch (state) {
		case RDO_SELECTED:
			setSelected(true);
			break;
		case RDO_UNSELECTED:
			setSelected(false);
			break;
		default:
			mState=state;
	}
}