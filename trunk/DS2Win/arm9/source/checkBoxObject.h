#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CCheckBox: public CWindowObject
{
protected:
	bool mPressed, mChecked;
public:
	CCheckBox(u16* map, u16* text, const GUIData* data, u16 ID);
	void processInput(u8 input);
	void setChecked(bool check);

	void draw();
};

