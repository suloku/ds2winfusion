#include "gui.h"

CGUI::CGUI(uint16* tileBase, uint16* mapBase, uint16* textBase, bool sub) {
	mTile = tileBase;
	mMap = mapBase;
	mText = textBase;
	mSub = sub;
	
	//The palette here is hard coded to be in the 3rd palette slot.
	//Needs to be changed in window.cpp and windowObject.cpp if you change this.
	if (sub)
		dmaCopy((uint16 *)guiGFXPal, (uint16 *)BG_PALETTE_SUB+48, 32);
	else
		dmaCopy((uint16 *)guiGFXPal, (uint16 *)BG_PALETTE+48, 32);
	
	//Copy the tile data
	dmaCopy((uint16 *)guiGFXData, (uint16 *)mTile, guiGFXLen);
	
	//I designed this to be able to use more than one window, but realistically it probably won't work.
	numWindows = windowCounter = 0;
}

void CGUI::processInput() {
	u32 down = keysDown(), up = keysUp(), held = keysHeld();
	int i;
	
	for (i = 0; i < numWindows; i++) {
		
		if(mWindows[i]->isVisible()) {
			if((down&KEY_TOUCH) && !mSub)
				mWindows[i]->processInput(TOUCH_DOWN);
			else if(down&KEY_B)
				mWindows[i]->processInput(KEY_B_DOWN);
			else if(down&KEY_A)
				mWindows[i]->processInput(KEY_A_DOWN);
			else if(down&KEY_UP)
				mWindows[i]->processInput(KEY_UP_DOWN);
			else if(down&KEY_DOWN)
				mWindows[i]->processInput(KEY_DOWN_DOWN);
			else if(down&KEY_LEFT)
				mWindows[i]->processInput(KEY_LEFT_DOWN);
			else if(down&KEY_RIGHT)
				mWindows[i]->processInput(KEY_RIGHT_DOWN);
			else if(down&KEY_START)
				mWindows[i]->processInput(KEY_START_DOWN);

			if((up&KEY_TOUCH) && !mSub)
				mWindows[i]->processInput(TOUCH_UP);

			if((held&KEY_TOUCH) && !mSub)
				mWindows[i]->processInput(TOUCH_HELD);
		}
	}

}


//This function should be changed to destroy any current windows,
//as calling it without calling destroyWindow on any open windows
//will cause problems.

CWindow* CGUI::createWindow(const WindowData *data) {
	CWindow *newWind = new CWindow (mMap, mText, data, windowCounter++);
	
//	if(numWindows>0)
//		destroyWindow(mWindows[0]);

	mWindows[numWindows++] = newWind;
//	mWindows[0] = newWind;

    return newWind;
}

void CGUI::destroyWindow(CWindow *wind) {
	CWindow *findWind;
	int i;
	u8 windNum=0;
	u16 ID = wind->getID();
	
	for (i = 0; i < numWindows; i++) {
		if(mWindows[i]->getID()==ID) {
			findWind = mWindows[i];
			windNum = i;
		}
	}

	for (i = windNum; i < numWindows-1; i++) {
		mWindows[i] = mWindows[i+1];
	}
	
	delete findWind;

	numWindows--;
}

