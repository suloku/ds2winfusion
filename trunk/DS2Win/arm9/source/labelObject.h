#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CLabel: public CWindowObject
{
private:
public:
	CLabel(u16* map, u16* text, const GUIData* data, u16 ID);
	void draw();
	void setText(char *title);
	void setTextCenter(char *title);
};

