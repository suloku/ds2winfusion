#include "window.h"

CWindow::CWindow (u16* map, u16* text, const WindowData* data, u16 ID) {
	mMap = map;
	mText = text;
	mID = ID;
	x1 = data->x1;
	y1 = data->y1;
	x2 = data->x2;
	y2 = data->y2;
	mWidth = x2-x1;
	mHeight = y2-y1;

	mNumObjects = 0;
	mState = WND_IDLE;
	mVisible = true;
	
	strncpy(mTitle, data->title, 32);

	drawWindow();
}

void CWindow::processInput(u8 input) {
	int i, j, numSelectors = 0, curSelector = 0;
	u8 result;
	CWindowObject *selectorList[32];
	
	if(input==KEY_B_DOWN)
		mState = WND_CANCEL;
	if(input==KEY_START_DOWN)
		mState = WND_OK;

	if(input == KEY_UP_DOWN || input == KEY_LEFT_DOWN || input == KEY_DOWN_DOWN || input == KEY_RIGHT_DOWN) {
		
		for (j = 0; j < mNumObjects; j++) {
			if(mObjects[j]->getType()==SELECTOR) {
				selectorList[numSelectors] = mObjects[j];
				if(mObjects[j]->getState() >= SEL_SELECTED) {
					curSelector=numSelectors;
					mObjects[j]->setState(SEL_UNSELECTED);
				}
				numSelectors++;
			}
		}
		
		if(numSelectors > 0) {
			if(input == KEY_UP_DOWN || input == KEY_LEFT_DOWN) {
				if(curSelector == 0)
					selectorList[numSelectors-1]->setState(SEL_SELECTED);
				else
					selectorList[curSelector-1]->setState(SEL_SELECTED);
			} else if(input == KEY_DOWN_DOWN || input == KEY_RIGHT_DOWN) {
				if(curSelector+1 >= numSelectors)
					selectorList[0]->setState(SEL_SELECTED);
				else
					selectorList[curSelector+1]->setState(SEL_SELECTED);							
			}
		}

	}

	for(i = 0; i < mNumObjects; i++) {
		result = mObjects[i]->getState();
		mObjects[i]->processInput(input);

		switch (mObjects[i]->getType()) {
			case SELECTOR:
				if(mObjects[i]->getState() == SEL_SELECTED && input == KEY_A_DOWN)
					mObjects[i]->setState(SEL_ACTIVATED);
				
				if(mObjects[i]->getState() == SEL_ACTIVATED)
					mState = WND_SEL;

				if(mObjects[i]->getState() == SEL_SELECTED && result != SEL_SELECTED) {
					for(j = 0; j < mNumObjects; j++) {
						if(mObjects[j]->getType() == SELECTOR && (mObjects[j]->getID() != mObjects[i]->getID()))
							mObjects[j]->setState(SEL_UNSELECTED);
					}
				}
				break;
			case RADIO:
				if(mObjects[i]->getState() == RDO_SELECTED && result != RDO_SELECTED) {
					for(j = 0; j < mNumObjects; j++) {
						if(mObjects[j]->getType() == RADIO && (mObjects[j]->getID() != mObjects[i]->getID()))
							mObjects[j]->setState(RDO_UNSELECTED);
					}
				}
				break;
			default:
				break;
		}
	}
}

CSelector *CWindow::addSelector (const GUIData* data) {
	CSelector *selector = new CSelector(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = selector;
	return selector;
}

CLabel *CWindow::addLabel(const GUIData* data) {
	CLabel *label = new CLabel(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = label;
	return label;
}

CGroupBox *CWindow::addGroupBox(const GUIData* data) {
	CGroupBox *group = new CGroupBox(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = group;
	return group;
}

CButton *CWindow::addButton(const GUIData* data) {
	CButton *button = new CButton(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = button;
	return button;
}

CIconButton *CWindow::addIconButton(const IconButtonData* data) {
	CIconButton *button = new CIconButton(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = button;
	return button;
}

CCheckBox *CWindow::addCheckBox(const GUIData* data) {
	CCheckBox *check = new CCheckBox(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = check;
	return check;
}
CRadioButton *CWindow::addRadioButton(const GUIData* data) {
	CRadioButton *radio = new CRadioButton(mMap, mText, data, mNumObjects);
	mObjects[mNumObjects++] = radio;
	return radio;
}

void CWindow::setText(char *title) {
	clearText(mText, x1+1, y1+1, x2-1, y1+1);
	strncpy(mTitle, title, 32);
	printOut(mText, mTitle, x1+1, y1+1, mWidth-2);
}

void CWindow::drawWindow () {
	u8 x,y;
	u16 tile = 0;
	
	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++) {
			if (x == x1) {
				if (y == y1)
					tile = TITLE_TOP_LEFT;
				else if (y == y1+1)
					tile = TITLE_LEFT;
				else if (y == y1+2)
					tile = TITLE_BOTTOM_LEFT;
				else if (y == y2)
					tile = WINDOW_BOTTOM_LEFT;
				else
					tile = WINDOW_LEFT;
			} else if (x == x2) {
				if (y == y1)
					tile = TITLE_TOP_RIGHT;
				else if (y == y1+1)
					tile = TITLE_RIGHT;
				else if (y == y1+2)
					tile = TITLE_BOTTOM_RIGHT;
				else if (y == y2)
					tile = WINDOW_BOTTOM_RIGHT;
				else
					tile = WINDOW_RIGHT;
			} else {
				if (y == y1)
					tile = TITLE_TOP;
				else if (y == y1+1)
					tile = TITLE_BLANK;
				else if (y == y1+2)
					tile = TITLE_BOTTOM;
				else if (y == y2)
					tile = WINDOW_BOTTOM;
				else
					tile = WINDOW_BLANK;
			}
			//Use palette slot 3
			tile |= (3 << 12);
			mMap[TILE(x,y)] = tile;
		}
	}

	printOut(mText, mTitle, x1+1, y1+1, mWidth-2);
}

void CWindow::setVisible(bool visible) {
	int i;

	mVisible = visible;
	
	if(visible)
		drawWindow();
	else
		hideWindow();

	for (i=0; i < mNumObjects; i++) {
		if(visible) {
			mObjects[i]->draw();
		} else {
			mObjects[i]->hide();
		}
	}
}

void CWindow::hideWindow() {
	u8 x,y;

	clearText(mText, x1+1,y1+1,x2-1,y1+1);

	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++)
			mMap[TILE(x,y)]=0;
	}
}


CWindow::~CWindow() {
	int i=0;
	
	hideWindow();

	for (i=0; i < mNumObjects; i++) {
		delete mObjects[i];
	}
}



/*void CWindow::drawSubWindow (char* title, u8 x1, u8 y1, u8 x2, u8 y2) {
	u8 width = x2-x1;
//	u8 height = y2-y1;
	u8 x,y;
	u16 tile = 0;
	
	for (y = y1; y <= y2; y++) {
		for (x = x1; x <= x2; x++) {
			if (x == x1) {
				if (y == y1)
					tile = SUB_TITLE_TOP_LEFT;
				else if (y == y1+1)
					tile = SUB_TITLE_LEFT;
				else if (y == y1+2)
					tile = SUB_TITLE_BOTTOM_LEFT;
				else if (y == y2)
					tile = SUB_WINDOW_BOTTOM_LEFT;
				else
					tile = SUB_WINDOW_LEFT;
			} else if (x == x2) {
				if (y == y1)
					tile = SUB_TITLE_TOP_RIGHT;
				else if (y == y1+1)
					tile = SUB_TITLE_RIGHT;
				else if (y == y1+2)
					tile = SUB_TITLE_BOTTOM_RIGHT;
				else if (y == y2)
					tile = SUB_WINDOW_BOTTOM_RIGHT;
				else
					tile = SUB_WINDOW_RIGHT;
			} else {
				if (y == y1)
					tile = SUB_TITLE_TOP;
				else if (y == y1+1)
					tile = SUB_TITLE_BLANK;
				else if (y == y1+2)
					tile = SUB_TITLE_BOTTOM;
				else if (y == y2)
					tile = SUB_WINDOW_BOTTOM;
				else
					tile = SUB_WINDOW_BLANK;
			}
			mMap[TILE(x,y)] = tile;
		}
	}
	if (strlen(title) > width-2)
		title[width-1]=0;

	iprintf ("\x1b[%d;%dH%s", y1+1, x1+1, title);
}*/

