#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CButton: public CWindowObject
{
protected:
	bool mPressed, mDown;
public:
	CButton(u16* map, u16* text, const GUIData* data, u16 ID);
	void processInput(u8 input);
	virtual void setDown(bool press);

	virtual void draw();
};

