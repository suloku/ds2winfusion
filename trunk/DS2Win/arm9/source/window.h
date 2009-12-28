#ifndef WINDOW_H
#define WINDOW_H

#include "nds.h"
#include <string.h>
#include "guiDef.h"
#include "text.h"
#include "windowObject.h"
#include "selectorObject.h"
#include "labelObject.h"
#include "groupBoxObject.h"
#include "buttonObject.h"
#include "iconButtonObject.h"
#include "checkBoxObject.h"
#include "radioButtonObject.h"

class CWindow
{
private:
	u16 *mMap, *mText;
	u16 mID;
	u8 x1,y1,x2,y2,mWidth,mHeight,mNumObjects,mState;
	char mTitle[32];
	bool mVisible;
	CWindowObject *mObjects[32];
public:
	CWindow(u16* map, u16* text, const WindowData* data, u16 ID);
	~CWindow();
	void drawWindow();
	void hideWindow();
	void processInput(u8 input);
	void setText(char *title);
	void setVisible(bool visible);

	CSelector* addSelector(const GUIData* data);
	CLabel* addLabel(const GUIData* data);
	CGroupBox* addGroupBox(const GUIData* data);
	CButton* addButton(const GUIData* data);
	CIconButton* addIconButton(const IconButtonData* data);
	CCheckBox* addCheckBox(const GUIData* data);
	CRadioButton* addRadioButton(const GUIData* data);

	u16 getID(){return mID;};
	u8 getState(){return mState;};
	bool isVisible(){return mVisible;};

//	void drawSubWindow(char* title, u8 x1, u8 y1, u8 x2, u8 y2);
//	void drawGroupBox(char* title, u8 x1, u8 y1, u8 x2, u8 y2);
};

#endif
