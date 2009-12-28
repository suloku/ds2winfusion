#pragma once

#include "nds.h"
#include "guiDef.h"
#include "buttonObject.h"
#include "windowObject.h"
#include "text.h"

class CWindowObject;

class CIconButton: public CButton
{
private:
	u16 mIcon;
public:
	CIconButton(u16* map, u16* text, const IconButtonData* data, u16 ID);
	void draw();
	void setDown(bool press);
};

