#ifndef GUI_H
#define GUI_H

#include <nds.h>
#include "window.h"

#include "guiDef.h"

#include "guiGFX.h"

class CWindow;

class CGUI
{
private:
	bool mSub;
	u16 *mTile, *mMap, *mText;
	u16 windowCounter;
	u8 numWindows;
	CWindow* mWindows[8];
public:
	CGUI(uint16* tileBase, uint16* mapBase, uint16* textBase, bool sub);
	~CGUI();
	CWindow* createWindow(const WindowData* data);
	void processInput();
	void destroyWindow(CWindow *wind);
};

#endif
