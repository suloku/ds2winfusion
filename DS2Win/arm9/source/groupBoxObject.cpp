#include "groupBoxObject.h"

CGroupBox::CGroupBox(u16* map, u16* text, const GUIData* data, u16 ID) : CWindowObject(map, text, data, ID) {
	draw();
	mType = LABEL;
}

void CGroupBox::draw() {
	u8 x,y;
	u16 tile = 0;

	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++) {
			if (x == x1) {
				if (y == y1)
					tile = GROUP_TOP_LEFT;
				else if (y == y2)
					tile = GROUP_BOTTOM_LEFT;
				else
					tile = GROUP_LEFT;
			} else if (x == x2) {
				if (y == y1)
					tile = GROUP_TOP_RIGHT;
				else if (y == y2)
					tile = GROUP_BOTTOM_RIGHT;
				else
					tile = GROUP_RIGHT;
			} else {
				if (y == y1) {
					if (x-x1 > strlen(mTitle))
						tile = GROUP_TOP;
					else
						tile = WINDOW_BLANK;
				} else if (y == y2)
					tile = GROUP_BOTTOM;
				else
					tile = GROUP_BLANK;
			}
			setTile(x,y,tile);
		}
	}
	printOut(mText, mTitle, x1+1, y1, x2-x1-1);
}

