#include "selectorObject.h"

CSelector::CSelector(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	mSelected = false;
	mState = SEL_UNSELECTED;
	mType = SELECTOR;

	draw();
}

void CSelector::processInput(u8 input) {
	switch (input) {
		case TOUCH_HELD:
			if (touchInside()) {
				setSelected(true);
				if (mState == SEL_UNSELECTED)
					mState = SEL_SELECTED;
			}
			break;
		case TOUCH_UP:
			if (lastInside()) {
				setSelected(true);
				mState = SEL_ACTIVATED;
			}
			break;
	}

//	if(mSelected && !touchInside())
//		setSelected(false);
}

void CSelector::setSelected(bool show) {
	u16 tile = WINDOW_BLANK;
	u8 x,y;
	
	mSelected = show;

	if (show)
		tile = SELECTED;

	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++)
			setTile(x,y,tile);
	}
}

void CSelector::setState(u8 state) {
	switch (state) {
		case SEL_SELECTED:
			setSelected(true);
			break;
		case SEL_UNSELECTED:
			setSelected(false);
			break;
	}
	mState=state;
}


