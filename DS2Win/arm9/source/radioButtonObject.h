#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CRadioButton: public CWindowObject
{
protected:
	bool mPressed, mSelected;
public:
	CRadioButton(u16* map, u16* text, const GUIData* data, u16 ID);
	void processInput(u8 input);
	void setSelected(bool sel);
	void setState(u8 state);

	void draw();
};

