#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CSelector: public CWindowObject
{
private:
	bool mSelected;
	void setSelected(bool show);
public:
	CSelector(u16* map, u16* text, const GUIData* data, u16 ID);
	void processInput(u8 input);
	void setState(u8 state);

//	void draw();
};

