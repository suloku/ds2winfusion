#pragma once

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CGroupBox: public CWindowObject
{
private:
public:
	CGroupBox(u16* map, u16* text, const GUIData* data, u16 ID);
	void draw();
};

