#pragma once

#include "nds.h"
#include <string.h>
#include "text.h"
#include "guiDef.h"

class CWindowObject
{
protected:
	u8 x1,y1,x2,y2, mType, mState, lastX, lastY;
	u16 mID;
	u16 *mMap, *mText;
	char mTitle[32];
public:
	CWindowObject(u16* map, u16* text, const GUIData* data, u16 ID);
	virtual ~CWindowObject();

	virtual void processInput(u8 input){};
	virtual u8 getState(){return mState;};
	virtual void setState(u8 state);
	virtual void draw();
	virtual void hide();
	void setText(char *title);
	void setTile(u8 x, u8 y, u16 tile);
	bool touchInside();
	bool lastInside();

	u8 getType(){return mType;};
	u8 getID(){return mID;};
};

