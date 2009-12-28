#include "checkBoxObject.h"

CCheckBox::CCheckBox(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	mPressed = false;
	mChecked = false;
	mState = CHK_UNCHECKED;
	mType = CHECKBOX;

	draw();
}

void CCheckBox::draw() {
	printOut(mText, mTitle, x1+2, y1, x2-x1-1);
	setChecked(mChecked);	
}

void CCheckBox::processInput(u8 input) {
	switch (input) {
		case TOUCH_DOWN:
			if (touchInside()) {
				mPressed = true;
				setTile(x1,y1,CHECKBOX_PRESSED);
			} break;
		case TOUCH_HELD:
			if (mPressed && touchInside())
				setTile(x1,y1,CHECKBOX_PRESSED);
			else if (mPressed && !touchInside())
				setChecked(mChecked);
			break;
		case TOUCH_UP:
			if (mPressed && lastInside())
				setChecked(!mChecked);

			mPressed = false;
			break;
	}
}

void CCheckBox::setChecked(bool check) {
	mChecked = check;
	if (mChecked) {
		mState = CHK_CHECKED;
		setTile(x1,y1,CHECKBOX_CHECKED);
	} else {
		mState = CHK_UNCHECKED;
		setTile(x1,y1,CHECKBOX_UNCHECKED);
	}
}

