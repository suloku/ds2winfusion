/*---------------------------------------------------------------------------------
Win2DS: By Bill Blaiklock (Sintax)

You are free to use aspects of this source in your own applications,
but please give credit where credit is due.
---------------------------------------------------------------------------------*/

#include <fat.h>
#include "nds.h"
#include <nds/arm9/console.h> //basic print funcionality
#include <nds/arm9/sound.h>	// sound functions
#include "clickdown_raw.h"
#include "clickup_raw.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <dswifi9.h>
#include <sys\socket.h>
#include <netinet\in.h>
#include <netdb.h>

#include "netdata.h"
#include "gui.h"
//#include "window.h"

#include "guiData.h"

#include "ascii_bin.h"
#include "text.h"

#include "keyboard.h"
#include "keyboard.def.h"

#include "client.h"
#include "screenshot.h"

#define		VCOUNT		(*((u16 volatile *) 0x04000006))

#define TEXT_XMAX		32
#define TEXT_YMAX		24

#define RGB(r,g,b) ((r) | ((g)<<5) | ((b)<<10))

#define MOUSEDELAY		2	//Frames to delay between sending mouse coordinates.

Wifi_AccessPoint global_connectAP;
CGUI *sGUI, *mGUI;
unsigned char global_wepkeys[4][32];
int global_wepkeyid, global_wepmode;
int global_dhcp; // 0=none, 1=get IP, 2=get DNS
unsigned long global_ipaddr, global_snmask, global_gateway, global_dns1,global_dns2;
unsigned long destip;

int screenSpeed = 3, screenPartSpeed = 2, partX = 0, partY = 0, zoom = 1;
bool leftHanded = false, mouseRelative = false, bindKeys = false, screenSwap = false, dualZoom = false;
touchPosition touchXY;
CScreenShot screenSmall, screenPart;

unsigned char customKeys[10] = {PGU, MLC, MMC, MRC, BSP, PGD, CRL, CRU, CRR, CRD};
const int dsKeys[10] = {KEY_L, KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_R, KEY_Y, KEY_X, KEY_A, KEY_B};

const u16 screenSpeedData[] = {
	0,
	1,
	5,
	50,
	100,
	200,
	400
};

const char * screenSpeedString[] = {
	"OFF",
	"ULTRA",
	"FASTER",
	"FAST",
	"NORMAL",
	"SLOW",
	"SLOWER"
};

const char * assocstatus_strings[] = {
	"DISCONNECTED",		// not *trying* to connect
	"SEARCHING",		// data given does not completely specify an AP, looking for AP that matches the data.
	"AUTHENTICATING",	// connecting...
	"ASSOCIATING",		// connecting...
	"ACQUIRING DHCP",	// connected to AP, but getting IP data from DHCP
	"ASSOCIATED",		// Connected!
	"CANNOT CONNECT",	// error in connecting...
};

enum gamePadKeys {
	GKEY_L,
	GKEY_LEFT,
	GKEY_UP,
	GKEY_RIGHT,
	GKEY_DOWN,
	GKEY_R,
	GKEY_Y,
	GKEY_X,
	GKEY_A,
	GKEY_B
};
const char * gamePadKeyStr[] = {
	"L",
	"LEFT",
	"UP",
	"RIGHT",
	"DOWN",
	"R",
	"Y",
	"X",
	"A",
	"B"
};

//Wifi functions
extern "C" {
void * sgIP_malloc(int size) { return malloc(size); }
void sgIP_free(void * ptr) { free(ptr); }
void sgIP_dbgprint(char * txt, ...) {
/*	char buffer[256];
	va_list args;
	va_start(args,txt);
	vsprintf(buffer,txt,args);
	iprintf (buffer);
	iprintf ("\n");*/
}
}
// wifi timer function, to update internals of sgIP
void Timer_50ms(void) {
   Wifi_Timer(50);
}

// notification function to send fifo message to arm7
void arm9_synctoarm7() { // send fifo message
   REG_IPC_FIFO_TX=0x87654321;
}

// interrupt handler to receive fifo messages from arm7
void arm9_fifo() { // check incoming fifo messages
   u32 value = REG_IPC_FIFO_RX;
   if(value == 0x87654321) Wifi_Sync();
}

//Dont think these are used any more considering I don't use iprintf any more, but they're here for reference I guess.
void printAddr (unsigned long addr){
	iprintf("%i.%i.%i.%i",(int)(addr&255),(int)((addr>>8)&255),(int)((addr>>16)&255),(int)((addr>>24)&255));
}
void printAddrLong (unsigned long addr){
	iprintf("%03i.%03i.%03i.%03i",(int)(addr&255),(int)((addr>>8)&255),(int)((addr>>16)&255),(int)((addr>>24)&255));
}

void waitInput () {
	do scanKeys(); while (!keysDown());
}

void swapBuffers (u16* buf1, u16* buf2, u16 size) {
	u16 i, tmp;
	for (i = 0; i < size; i++) {
		tmp = buf1[i];
		buf1[i] = buf2[i];
		buf2[i] = tmp;
	}
}

void waitVbl() {
	scanKeys();
	touchXY=touchReadXY();
	
	static s8 lid_state = -1; //start the lid_state at null
	if (IPC->buttons & IPC_LID_CLOSED) {
		if (lid_state != 1) {
			lid_state = 1; //change state, disable power
			powerOFF(POWER_ALL_2D);
		}
	} else {
		if (lid_state != 0) {
			lid_state = 0; //change state and enable power
			powerON(POWER_ALL_2D);
		}
	}
}

char *printKey (unsigned char key) {
	char tmp[6];
	switch (key) {
		case F_1:
			return "F1"; break;
		case F_2:
			return "F2"; break;
		case F_3:
			return "F3"; break;
		case F_4:
			return "F4"; break;
		case F_5:
			return "F5"; break;
		case F_6:
			return "F6"; break;
		case F_7:
			return "F7"; break;
		case F_8:
			return "F8"; break;
		case F_9:
			return "F9"; break;
		case F10:
			return "F10"; break;
		case F11:
			return "F11"; break;
		case F12:
			return "F12"; break;
		case EXT:
			return "EXIT"; break;
		case HOM:
			return "HOME"; break;
		case PGU:
			return "PAGEUP"; break;
		case PGD:
			return "PAGEDOWN"; break;
		case END:
			return "END"; break;
		case TAB:
			return "TAB"; break;
		case ESC:
			return "ESCAPE"; break;
		case BSP:
			return "BCKSPACE"; break;
		case CAP:
			return "CAPSLOCK"; break;
		case RET:
			return "ENTER"; break;
		case CTL:
			return "CONTROL"; break;
		case ALT:
			return "ALT"; break;
		case NDS:
			return "WIN"; break;
		case CRU:
			return "UP"; break;
		case CRD:
			return "DOWN"; break;
		case CRL:
			return "LEFT"; break;
		case CRR:
			return "RIGHT"; break;
		case INS:
			return "INSERT"; break;
		case DEL:
			return "DEL"; break;
		case SHF:
			return "SHIFT"; break;
		case SCN:
			return "SCREEN"; break;
		case SPC:
			return "SPACE"; break;
		case MLC:
			return "L-MOUSE"; break;
		case MMC:
			return "M-MOUSE"; break;
		case MRC:
			return "R-MOUSE"; break;
		case ZOM:
			return "SET ZOOM"; break;
		case SWP:
			return "SCRNSWAP"; break;
		default:
			sprintf (tmp, "%c", key);
			return tmp;
			break;
	}
	return 0;
}

void gamePadMode() {
	u16 delay = 0;
	int i;

	CIconButton *OKButton, *cancelButton;
	CWindow *infoWind = mGUI->createWindow(&WND_GAMEPAD);

	OKButton = infoWind->addIconButton(&BTN_GAMEPADOK);
	cancelButton = infoWind->addIconButton(&BTN_GAMEPADCANCEL);
	infoWind->addLabel(&LBL_GAMEPAD1);
	infoWind->addLabel(&LBL_GAMEPAD2);
	infoWind->addLabel(&LBL_GAMEPAD3);
	infoWind->addLabel(&LBL_GAMEPAD4);
	infoWind->addLabel(&LBL_GAMEPAD5);
	infoWind->addLabel(&LBL_GAMEPAD6);
	infoWind->addLabel(&LBL_GAMEPAD7);

	do {
		swiWaitForVBlank();
		updateConnection();

		mGUI->processInput();

		if(cancelButton->getState()==BTN_ACTIVATED) {
			mGUI->destroyWindow(infoWind);
			return;
		}
	} while(infoWind->getState()!=WND_OK && OKButton->getState()!=BTN_ACTIVATED);

	mGUI->destroyWindow(infoWind);
	
	powerOFF(POWER_ALL_2D);

	do {
		swiWaitForVBlank();
		updateConnection();
        delay++;
		
		for (i = 0; i < 10; i++) {
			if (keysDown()&dsKeys[i]) {
				switch (customKeys[i]) {
					case MLC: sendCommand (MOUSECLICKDOWN); break;
					case MMC: sendCommand (MOUSEMCLICKDOWN); break;
					case MRC: sendCommand (MOUSERCLICKDOWN); break;
					default: sendVirtualKeyDown(customKeys[i]);
				}
			} else if (keysUp()&dsKeys[i]) {
				switch (customKeys[i]) {
					case MLC: sendCommand (MOUSECLICKUP); break;
					case MMC: sendCommand (MOUSEMCLICKUP); break;
					case MRC: sendCommand (MOUSERCLICKUP); break;
					default: sendVirtualKeyUp(customKeys[i]);
				}
			} else if (keysHeld()&dsKeys[i]) {
				switch (customKeys[i]) {
					case MLC: 
					case MMC: 
					case MRC: 
					case ZOM:
					case SWP: break;
					default: if (delay > 5) {//sendKey (customKeys[i]);
					//sendVirtualKeyDown(customKeys[i]);
					//sendVirtualKeyUp(customKeys[i]);
					} break;
				}
			}
		}
		
		if (touchXY.px > 0 && touchXY.py > 0) {
		//	if (delay > MOUSEDELAY) {
				if (keysDown()&KEY_TOUCH)
					sendMouseRelStart (touchXY.px, touchXY.py);

				sendMouseMove (touchXY.px, touchXY.py, true);
		//	}
		}
		
		if (delay > 5) delay = 0; // do this outside other loops so multiple keys held down will be recorded together

	} while ( !((keysHeld()&KEY_START) && (keysHeld()&KEY_SELECT)) );
	
	powerON(POWER_ALL_2D);

	return;
}

void bindMenu(unsigned char* keys) {
	char ip[8] = "";
	char keybd, str[16] = "";
	int menuSel = 0, x, y, i=0;
	bool changed = true;
//	bool showMenu = false, showOnce = true;
	
	GUIData SEL_KEYS = {
		0,0,0,0, ""};
	GUIData LBL_KEYS = {
		0,0,0,0, ""};

	CWindow *customWnd = sGUI->createWindow(&WND_CUSTOMKEYS);
	CSelector *selectors[10];

	customWnd->addLabel(&LBL_CUSTOM1);
	customWnd->addLabel(&LBL_CUSTOM2);
	customWnd->addLabel(&LBL_CUSTOM3);
	
	for (x = 3; x < 20; x += 15) {
		for (y = 6; y < 16; y += 2) {
			LBL_KEYS.x1 = x;
			LBL_KEYS.x2 = x+4;
			LBL_KEYS.y1 = LBL_KEYS.y2 = y;
			if(x < 15) {
				SEL_KEYS.x1 = x+5;
				SEL_KEYS.x2 = x+14;
			} else {
				SEL_KEYS.x1 = x+2;
				SEL_KEYS.x2 = x+11;
			}
			SEL_KEYS.y1 = SEL_KEYS.y2 = y;
			sprintf (LBL_KEYS.title, "%s: ", gamePadKeyStr[i]);

			customWnd->addLabel(&((const GUIData)LBL_KEYS));
			selectors[i]=customWnd->addSelector(&((const GUIData)SEL_KEYS));
			
			selectors[i]->setText(printKey (keys[i]));

			i++;
		}
	}
	
	selectors[0]->setState(SEL_SELECTED);

	showKeyboard(true);
	
	do {
		swiWaitForVBlank();
		
		updateConnection();

//		if (showMenu) {
		keybd = processKeyboard(&str[0], 15, 1);
		
		selectors[menuSel]->setState(SEL_UNSELECTED);

		if (keysDown()&KEY_UP)
			menuSel--;
		else if (keysDown()&KEY_DOWN)
			menuSel++;
		else if (keysDown()&KEY_LEFT)
			menuSel-=5;
		else if (keysDown()&KEY_RIGHT)
			menuSel+=5;

		if (menuSel < 0)
			menuSel +=10;
		if (menuSel > 9)
			menuSel %= 5;
		
		selectors[menuSel]->setState(SEL_SELECTED);

		if (keybd != 0) {
			keys[menuSel] = keybd;
			changed = true;
		}
		if (keysDown()&KEY_Y) {
			keys[menuSel] = MLC; changed = true;
		} else if (keysDown()&KEY_X) {
			keys[menuSel] = MMC; changed = true;
		} else if (keysDown()&KEY_A) {
			keys[menuSel] = MRC; changed = true;
		} else if (keysDown()&KEY_L) {
			keys[menuSel] = ZOM; changed = true;
		} else if (keysDown()&KEY_R) {
			keys[menuSel] = SWP; changed = true;
		}

		if (changed) {
			changed = false;
			selectors[menuSel]->setText(printKey (keys[menuSel]));
		}

//		} else {
//			for (i = 0; i < 10; i++) {
//				if (keysDown()&dsKeys[i])
//					sendKeyDown(keys[i]);
//				else if (keysUp()&dsKeys[i])
//					sendKeyUp(keys[i]);
//			}
//		}
//		if (keysDown()&KEY_START || showOnce) 
//			showMenu = !showMenu;
//			showOnce = false;
//			if (showMenu) {
///			} else {
//				clearScreen();
//				showKeyboard(false);
//			}
//		}
	} while (!(keysDown()&KEY_START));

	FILE* fFile = fopen ("DS2Win.ini", "wb");		
	
	if (fFile) {
	  sprintf(ip,"%X",destip);
	  fputs(ip, fFile);
	  for (i = 0; i < 10; i++) 
	    fputc(keys[i],fFile);			    
	  fclose(fFile);
	  }

	showKeyboard(false);
	sGUI->destroyWindow(customWnd);
}

void virtualDesktopMode() {
	u16 delay = 0;
	bool showkeys = false, showMenu = false, quit = false, tDown, tUp;
	char keybd, str[16] = "";
	int i, kDown, kUp, kHeld, kLast, zoomBtn = 0;
	touchPosition tLast;

	CWindow *mainWnd;
	CButton *topLeftBtn, *topRightBtn, *btmLeftBtn, *btmRightBtn, *mainMenuBtn, *OKButton;
	CRadioButton *leftKeysBtn, *rightKeysBtn, *customKeysBtn;
	CCheckBox *mouseCheck;
	CLabel *topSpeedLbl, *btmSpeedLbl;
	
	screenSmall.setComplete (false);
	screenPart.setComplete (false);
	kLast = keysHeld();
	tLast = touchReadXY();

	do {
		swiWaitForVBlank();
		
		while (updateConnection() > 0) {
			processScreenShot(&screenSmall);
			processScreenShotPart(&screenPart);
			delay = 8;
		}

		setZoom(zoom);
        if (!(IPC->buttons & IPC_LID_CLOSED) && !showMenu) {
			if (screenSwap) {
			    if (dualZoom) // update at same speed as zoomed screen
				  updateScreenShot(&screenSmall, screenSpeedData[screenPartSpeed], SCREENPARTVRAM, dualZoom);
				else
				  updateScreenShot(&screenSmall, screenSpeedData[screenSpeed], SCREENPARTVRAM, dualZoom);
		
				if (!showkeys) // only update top screen if keyboard is showing for performance
					updateScreenShotPart(&screenPart, screenSpeedData[screenPartSpeed], (u8) partX, (u8) partY, SCREENVRAM);
			}
			else if (!screenSwap) {
				updateScreenShotPart(&screenPart, screenSpeedData[screenPartSpeed], (u8) partX, (u8) partY, SCREENPARTVRAM);
				if (!showkeys) // only update top screen if keyboard is showing for performance
					updateScreenShot(&screenSmall, screenSpeedData[screenSpeed], SCREENVRAM, dualZoom); 			
			}
		}
		
		kDown = keysDown();
		kUp   = keysUp();
		kHeld = keysHeld();
		if (kHeld != kLast) {
			kDown = (kHeld^kLast)&kHeld;     // if we miss keydown this will give us keydown
			kUp   = (kHeld^kLast)&(~kHeld);  // if we miss keyup this will give us keyup
		}
		kLast = kHeld;

		tDown = false;
		tUp = false;

		if (touchXY.px > 0 && touchXY.py > 0 && tLast.px == 0 && tLast.py == 0) {
			tDown = true;
		}
		else if (touchXY.px == 0 && touchXY.py == 0 && tLast.px > 0 && tLast.py > 0) {
			tUp = true;
		}

		tLast.px = touchXY.px;
		tLast.py = touchXY.py;		

		if (showkeys) {
			//Process keyboard
			keybd = processKeyboard(&str[0], 15, 1);			
			if (!showMenu && keybd != 0) sendKey (keybd);
		}
		else if (!showMenu){
			//Send Mouse controls

			if (!showkeys) { //(delay > MOUSEDELAY)
				if (mouseRelative && touchXY.px > 0 && touchXY.py > 0) {
					if (kDown&KEY_TOUCH) sendMouseRelStart (touchXY.px, touchXY.py);
					sendMouseMove (touchXY.px, touchXY.py, true);
				}
				else {
					if (screenSwap) {
						if (touchXY.px > 0 && touchXY.py > 0)
							sendMouseMoveZoom (partX, partY, touchXY.px, touchXY.py);
						if (!(kHeld&dsKeys[zoomBtn])) { // if changing zoomed screen position disable clicking
							if (tDown) {
								sendCommand (MOUSECLICKDOWN);
								playGenericSound(clickdown_raw, clickdown_raw_size);
							}
							if (tUp) {
								sendCommand (MOUSECLICKUP);
								playGenericSound(clickup_raw, clickup_raw_size);
							}
						}
					}
					else if (touchXY.px > 0 && touchXY.py > 0)
						sendMouseMove (touchXY.px, touchXY.py, false);
				}
			
			}

			if (bindKeys) {
				for (i = 0; i < 10; i++) {
					if (kDown&dsKeys[i]) {
						switch (customKeys[i]) {
							case MLC:
								sendCommand (MOUSECLICKDOWN); break;
							case MMC:
								sendCommand (MOUSEMCLICKDOWN); break;
							case MRC:
								sendCommand (MOUSERCLICKDOWN); break;
							case ZOM:
								zoomBtn = i;
								break;
							case SWP:
								swapBuffers (SCREENVRAM, SCREENPARTVRAM, 49152);
								screenSwap = !screenSwap;
								break;
							default:
								sendVirtualKeyDown(customKeys[i]); break;
						}
					} else if (kUp&dsKeys[i]) {
						switch (customKeys[i]) {
							case MLC:
								sendCommand (MOUSECLICKUP); break;
							case MMC:
								sendCommand (MOUSEMCLICKUP); break;
							case MRC:
								sendCommand (MOUSERCLICKUP); break;
							case ZOM:
							case SWP: break;
							default:
								sendVirtualKeyUp(customKeys[i]); break;
						}
					}
				}

				if (customKeys[zoomBtn] == ZOM && (kHeld&dsKeys[zoomBtn])) {
					if (!showkeys && touchXY.px > 0 && touchXY.py > 0) {
						partX = touchXY.px;//-25; -aj
						partY = touchXY.py;//-20; -aj
					}
				}

			} else {
				// Change Zoom level
				if ((!leftHanded&&(kHeld&KEY_R))||(leftHanded&&(kHeld&KEY_L))) {
					if ((!leftHanded&&(kDown&KEY_Y))||(leftHanded&&(kDown&KEY_LEFT))) // min zoom
						zoom = 1;
					if ((!leftHanded&&(kDown&KEY_A))||(leftHanded&&(kDown&KEY_RIGHT))) // max zoom
						zoom = 8;
					if ((!leftHanded&&(kDown&KEY_X))||(leftHanded&&(kDown&KEY_UP))) // inc zoom
						zoom++;
					if ((!leftHanded&&(kDown&KEY_B))||(leftHanded&&(kDown&KEY_DOWN))) // dec zoom
						zoom--;

					if (zoom > 8) zoom = 8;
					if (zoom < 1) zoom = 1;
				}
				//Swap Screens
				if ((!leftHanded&&(kDown&KEY_R)&&!(kHeld&KEY_L))||(leftHanded&&(kDown&KEY_L)&&!(kHeld&KEY_R))) {
				    if (screenSwap&&!dualZoom)
						dualZoom = true;
					else {
						swapBuffers (SCREENVRAM, SCREENPARTVRAM, 49152);
						screenSwap = !screenSwap;
						dualZoom = false;
						}
				}
				//Move Zoomed
				if (touchXY.px > 0 && touchXY.py > 0 && ((!leftHanded&&(kHeld&KEY_L))||(leftHanded&&(kHeld&KEY_R)))) {
					partX = touchXY.px; //-25 -aj
					partY = touchXY.py; //-20 -aj
				}
				
				//Mouse Commands
				if ((!leftHanded && (kDown&KEY_LEFT)) || (leftHanded && (kDown&KEY_Y)))
					sendCommand (MOUSECLICKDOWN);
				else if ((!leftHanded && (kUp&KEY_LEFT)) || (leftHanded && (kUp&KEY_Y)))
					sendCommand (MOUSECLICKUP);
				if ((!leftHanded && (kDown&KEY_RIGHT)) || (leftHanded && (kDown&KEY_A)))
					sendCommand (MOUSERCLICKDOWN);
				else if ((!leftHanded && (kUp&KEY_RIGHT)) || (leftHanded && (kUp&KEY_A)))
					sendCommand (MOUSERCLICKUP);
				if ((!leftHanded && (kDown&KEY_UP)) || (leftHanded && (kDown&KEY_X)))
					sendCommand (MOUSEMCLICKDOWN);
				else if ((!leftHanded && (kUp&KEY_UP)) || (leftHanded && (kUp&KEY_X)))
					sendCommand (MOUSEMCLICKUP);
			}
		}
		
		if (delay < 8) delay++;
		
		if ((!leftHanded&&(kHeld&KEY_L))||(leftHanded&&(kHeld&KEY_R))) {
		    if (delay > 3) {
				if ((!leftHanded&&(kHeld&KEY_Y))||(leftHanded&&(kHeld&KEY_LEFT))) // cursor keys
					sendKey (CRL);
				if ((!leftHanded&&(kHeld&KEY_A))||(leftHanded&&(kHeld&KEY_RIGHT)))
					sendKey (CRR);
				if ((!leftHanded&&(kHeld&KEY_X))||(leftHanded&&(kHeld&KEY_UP)))
					sendKey (CRU);
				if ((!leftHanded&&(kHeld&KEY_B))||(leftHanded&&(kHeld&KEY_DOWN)))
					sendKey (CRD);
				delay=0;
				}
		}
		else {
			if ((!leftHanded&&(kHeld&KEY_Y))||(leftHanded&&(kHeld&KEY_LEFT))) //Scroll zoomed view
				partX -= 2;
			if ((!leftHanded&&(kHeld&KEY_A))||(leftHanded&&(kHeld&KEY_RIGHT)))
				partX += 2;
			if ((!leftHanded&&(kHeld&KEY_X))||(leftHanded&&(kHeld&KEY_UP)))
				partY -= 2;
			if ((!leftHanded&&(kHeld&KEY_B))||(leftHanded&&(kHeld&KEY_DOWN)))
				partY += 2;
		}			 

		if (partX < 0) partX = 0;
		else if (partX > 255) partX = 255;
		if (partY < 0) partY = 0;
		else if (partY > 191) partY = 191;
		
		if (showMenu) {
			mGUI->processInput();

			if (topLeftBtn->getState()==BTN_ACTIVATED) {
				topLeftBtn->setState(BTN_UP);
				screenPartSpeed++;
				if(screenPartSpeed > 6) screenPartSpeed = 0;
				topSpeedLbl->setTextCenter((char*)screenSpeedString[screenPartSpeed]);
			} else if (topRightBtn->getState()==BTN_ACTIVATED) {
				topRightBtn->setState(BTN_UP);
				screenPartSpeed--;
				if(screenPartSpeed < 0) screenPartSpeed = 6;
				topSpeedLbl->setTextCenter((char*)screenSpeedString[screenPartSpeed]);
			} else if (btmLeftBtn->getState()==BTN_ACTIVATED) {
				btmLeftBtn->setState(BTN_UP);
				screenSpeed++;
				if(screenSpeed > 5) screenSpeed = 0;
				btmSpeedLbl->setTextCenter((char*)screenSpeedString[screenSpeed]);
			} else if (btmRightBtn->getState()==BTN_ACTIVATED) {
				btmRightBtn->setState(BTN_UP);
				screenSpeed--;
				if(screenSpeed < 0) screenSpeed = 5;
				btmSpeedLbl->setTextCenter((char*)screenSpeedString[screenSpeed]);
			
			} else if (mainMenuBtn->getState()==BTN_ACTIVATED) {
				mainMenuBtn->setState(BTN_UP);
				quit = true;
			}
		} 

		//Show/hide keyboard
		if (((kDown&KEY_SELECT) || (showkeys && keybd==EXT)) && !showMenu) {
			showkeys = !showkeys;
			showKeyboard(showkeys);
		}
		if (kDown&KEY_START || (showMenu && OKButton->getState()==BTN_ACTIVATED)) {
			showMenu = !showMenu;
			if (showMenu) {

				showkeys = false;
				showKeyboard(false);

				mainWnd = mGUI->createWindow(&WND_DESKTOP);
				topLeftBtn = mainWnd->addIconButton(&BTN_TOPLEFT);
				topRightBtn = mainWnd->addIconButton(&BTN_TOPRIGHT);
				btmLeftBtn = mainWnd->addIconButton(&BTN_BOTTOMLEFT);
				btmRightBtn = mainWnd->addIconButton(&BTN_BOTTOMRIGHT);
				mainMenuBtn = mainWnd->addButton(&BTN_DESKTOPQUIT);
				OKButton = mainWnd->addIconButton(&BTN_DESKTOPOK);
				mainWnd->addGroupBox(&GRP_KEYS);
				leftKeysBtn = mainWnd->addRadioButton(&RDO_KEYSLEFT);
				rightKeysBtn = mainWnd->addRadioButton(&RDO_KEYSRIGHT);
				customKeysBtn = mainWnd->addRadioButton(&RDO_KEYSCUSTOM);
				mouseCheck = mainWnd->addCheckBox(&CHK_MOUSEREL);
				topSpeedLbl = mainWnd->addLabel(&LBL_TOPSPEED);
				btmSpeedLbl = mainWnd->addLabel(&LBL_BOTTOMSPEED);
				mainWnd->addLabel(&LBL_TOPLABEL);
				mainWnd->addLabel(&LBL_BTMLABEL);
				
				topSpeedLbl->setTextCenter((char*)screenSpeedString[screenPartSpeed]);
				btmSpeedLbl->setTextCenter((char*)screenSpeedString[screenSpeed]);

				if(bindKeys)
					customKeysBtn->setState(RDO_SELECTED);
				else {
					if(leftHanded)
						leftKeysBtn->setState(RDO_SELECTED);
					else
						rightKeysBtn->setState(RDO_SELECTED);
				}
				if(mouseRelative)
					mouseCheck->setChecked(true);
				else
					mouseCheck->setChecked(false);
			} else {
				if(customKeysBtn->getState()==RDO_SELECTED)
					bindKeys = true;
				else {
					bindKeys = false;
					if(leftKeysBtn->getState()==RDO_SELECTED)
						leftHanded = true;
					else leftHanded = false;
				}
				if(mouseCheck->getState()==CHK_CHECKED)
					mouseRelative = true;
				else
					mouseRelative = false;

				mGUI->destroyWindow(mainWnd);
			}
		}
	} while (!quit);

	if(showMenu)
		mGUI->destroyWindow(mainWnd);
}

bool validatePassword() {
	u16 passAccepted;
	CWindow *connWnd;
	CLabel *connLbl;
	CIconButton *cancelButton;
	bool connected = false;
	
	connWnd = mGUI->createWindow(&WND_PASSSTATUS);
	connLbl = connWnd->addLabel(&LBL_PASSSTATUS);
	cancelButton = connWnd->addIconButton(&BTN_PASSCANCEL);

	do {
		swiWaitForVBlank();
		mGUI->processInput();

		passAccepted = isPassAccepted();
		if (passAccepted == 1) {
			connLbl->setText("Password accepted.");
			connected = true;
		} else if (passAccepted == 0)
			connLbl->setText("Password rejected.");

	} while (!connected && connWnd->getState()!=WND_CANCEL && cancelButton->getState()!=BTN_ACTIVATED);
	
	mGUI->destroyWindow(connWnd);
	return connected;
}

void sendTouchUDP() {
    FILE* fFile;
	char str[16] = "";
	char ip[8] = "";
	char keybd;
	bool connected = false;
	int sel, i;

	CWindow *passWnd, *mainWnd;
	CLabel *passLbl;
	CSelector *selDesktop, *selCustomize, *selGamePad, *selDisconnect, *selReconnect;

	openSocket (8888);

	//Password Sending//
	do {
		
		passWnd = sGUI->createWindow(&WND_PASSWORD);
		passLbl = passWnd->addLabel(&LBL_PASSWORD);
		
		showKeyboard(true);

		while (((keybd = processKeyboard(&str[0], 15, 0) != RET) && !(keysDown()&KEY_A)) && passWnd->getState()!=WND_OK) {
			swiWaitForVBlank();
			sGUI->processInput();
			passLbl->setText(str);
			if (keybd == ESC || passWnd->getState()==WND_CANCEL) {
				sGUI->destroyWindow(passWnd);
				showKeyboard(false);
				return;
			}
		}
		
		sendPassword (str, destip);
		sGUI->destroyWindow(passWnd);
		showKeyboard(false);
		
		connected = validatePassword();
		
		if (connected) {
			FILE* fFile = fopen ("DS2Win.ini", "wb");		
			
			if (fFile) {
			  sprintf(ip,"%X",destip);
			  fputs(ip, fFile);
			  for (i = 0; i < 10; i++) 
			    fputc(customKeys[i],fFile);			    
			  fclose(fFile);
			  }
		}
	} while (!connected);

	
	do {
		mainWnd = mGUI->createWindow(&WND_CONNMAIN);
		selDesktop = mainWnd->addSelector(&SEL_DESKTOP);
		selGamePad = mainWnd->addSelector(&SEL_GAMEPAD);
		selCustomize = mainWnd->addSelector(&SEL_CUSTOMKEYS);
		selDisconnect = mainWnd->addSelector(&SEL_DISCONNECT);
		selReconnect = mainWnd->addSelector(&SEL_RECONNECT);
		selDesktop->setState(SEL_SELECTED);
		
		sel = 0;

		do {
			swiWaitForVBlank();
			mGUI->processInput();
			
			updateConnection();
		} while (mainWnd->getState()!=WND_SEL);

		if(selDesktop->getState()==SEL_ACTIVATED)			sel = 1;
		else if(selCustomize->getState()==SEL_ACTIVATED)	sel = 2;
		else if(selGamePad->getState()==SEL_ACTIVATED)		sel = 3;
		else if(selDisconnect->getState()==SEL_ACTIVATED)	sel = 4;
		else if(selReconnect->getState()==SEL_ACTIVATED)	sel = 5;

		mGUI->destroyWindow(mainWnd);
		
		switch (sel) {		
			case 1: virtualDesktopMode(); break;
			case 2: bindMenu (customKeys); break;
			case 3: gamePadMode(); break;
			case 4: connected = false; break;
			case 5: {sendPassword (str, destip); validatePassword();} 
		}

	} while (connected);
	
	clientDisconnect();
	strcpy (str, "");	
}


bool selectAP() {
	int numAP = 0, i, lastNumAP = 0, sel = 0;
	Wifi_AccessPoint ap;
	char tmpTxt[32];

	CWindow *selWnd;
	CSelector *selectors[18];
	CLabel *lblNumAP;

	GUIData SEL_AP = {
		6, 4, 24, 4, ""};


	selWnd = mGUI->createWindow(&WND_SELAP);
	lblNumAP = selWnd->addLabel(&LBL_NUMAP);

	Wifi_EnableWifi();
	Wifi_ScanMode();

	do {
		swiWaitForVBlank();
		mGUI->processInput();

		numAP = Wifi_GetNumAP();

		if(lastNumAP != numAP) {
			snprintf (tmpTxt, 32, "APs found: %i", numAP);
			lblNumAP->setText(tmpTxt);
			
			for (i = lastNumAP; i <= numAP-1 && i < 18; i++) {
				if(Wifi_GetAPData(i,&ap)==WIFI_RETURN_OK)
					strncpy(SEL_AP.title, ap.ssid, 32);
				else
					strcpy(SEL_AP.title, "");
				SEL_AP.y1=SEL_AP.y2= 4+i;
				selectors[i] = selWnd->addSelector(&((const GUIData)SEL_AP));
			}

		}
		lastNumAP = numAP;

		if (selWnd->getState()==WND_CANCEL) {
			mGUI->destroyWindow(selWnd);
			return false;
		}

	} while (selWnd->getState() != WND_SEL || numAP == 0);
	
	for (i = 0; i < numAP; i++) {
		if(selectors[i]->getState()==SEL_ACTIVATED)
			sel = i;
	}

	mGUI->destroyWindow(selWnd);
	Wifi_GetAPData(sel,&global_connectAP);
	return true;
}

bool enterIP(unsigned long *destip, char *name) {
	unsigned long tmpData = *destip;
	u8* ptr = (u8*) &tmpData;
	
	CSelector *selectors[4];
	CButton *numButtons[10];
	CIconButton *OKButton, *cancelButton;

	int i, sel = 0, lastSel = 0, numSel = 0;
	u8 x = 11;
	u8 y = 9;
	char numTxt[4] = "000";

	GUIData SEL_IP = {
		8, 6, 10, 6, "000"};
	GUIData BTN_NUM = {
		14, 18, 16, 20, " 0 "};
	GUIData LBL_IP = {
		11, 6, 11, 6, "."};
	GUIData GRP_SETIP = {
		7, 5, 23, 7, "" };

	CWindow *IPWindow = mGUI->createWindow(&WND_SETIP);
	
	for (i = 0; i < 4; i++) {
		sprintf(SEL_IP.title, "%03i", ptr[i]);
		selectors[i] = IPWindow->addSelector(&((const GUIData)SEL_IP));
		SEL_IP.x1+=4;
		SEL_IP.x2+=4;

		if (i<3)
			IPWindow->addLabel(&((const GUIData)LBL_IP));
		LBL_IP.x1+=4;
		LBL_IP.x2+=4;
	}
	
	strncpy(GRP_SETIP.title, name, 32);
	IPWindow->addGroupBox(&((const GUIData)GRP_SETIP));
	
	selectors[0]->setState(SEL_SELECTED);

	OKButton = IPWindow->addIconButton(&BTN_SETIPOK);
	cancelButton = IPWindow->addIconButton(&BTN_SETIPCANCEL);

	numButtons[0] = IPWindow->addButton(&((const GUIData)BTN_NUM));
	for (i = 1; i < 10; i++) {
		BTN_NUM.x1 = x;
		BTN_NUM.x2 = x+2;
		BTN_NUM.y1 = y;
		BTN_NUM.y2 = y+2;
		sprintf(BTN_NUM.title, "%i", i);
		
		numButtons[i] = IPWindow->addButton(&((const GUIData)BTN_NUM));

		x+=3;
		if (x > 19) {
			x = 11;
			y+=3;
		}
	}
	
	do {
		swiWaitForVBlank();
		mGUI->processInput();

		for (i = 0; i < 4; i++) {
			if(selectors[i]->getState() >= SEL_SELECTED) {
				lastSel = sel;
				sel = i;
			}
		}
		for (i = 0; i < 10; i++) {
			if(numButtons[i]->getState()==BTN_ACTIVATED) {
				numButtons[i]->setState(BTN_UP);
				if(numSel == 0)
					ptr[sel]=0;
				
				ptr[sel]=(ptr[sel]*10)+i;

				sprintf(numTxt, "%03i", ptr[sel]);
				selectors[sel]->setText(numTxt);
                numSel++;
			}
		}
		if (numSel > 2) {
			numSel = 0;
			selectors[sel]->setState(SEL_UNSELECTED);
			sel++;
			if(sel > 3)
				sel = 0;
			selectors[sel]->setState(SEL_SELECTED);
		}

		if (lastSel != sel)
			numSel = 0;

		if(IPWindow->getState()==WND_CANCEL || cancelButton->getState()==BTN_ACTIVATED) {
			mGUI->destroyWindow(IPWindow);
			return false;
		}
	} while(IPWindow->getState()!=WND_OK && OKButton->getState()!=BTN_ACTIVATED  && !(keysDown()&KEY_A));
	
	*destip = tmpData;

	mGUI->destroyWindow(IPWindow);
	
	return true;
}

bool connectionMenu() {
	u8* ptr = (u8*) &global_ipaddr;
	int i, y, sel;
	
	GUIData SEL_CONNSET = {
		9, 4, 23, 4, ""};
	
	CSelector *selectors[5];
	CWindow *connWindow;
	CButton *OKButton, *cancelButton, *changeButton;
	
	while (true) {
		sel = -1;
		y = 4;

		connWindow = mGUI->createWindow(&WND_CONNSETTINGS);
		connWindow->addGroupBox(&GRP_SETIP);
		connWindow->addGroupBox(&GRP_SETGATEWAY);
		connWindow->addGroupBox(&GRP_SETDNS1);
		connWindow->addGroupBox(&GRP_SETDNS2);
		connWindow->addGroupBox(&GRP_SETSUBNET);
		OKButton = connWindow->addIconButton(&BTN_CONNOK);
		cancelButton = connWindow->addIconButton(&BTN_CONNCANCEL);
		changeButton = connWindow->addButton(&BTN_CONNCHANGE);
		
		for (i = 0; i < 5; i++) {
			SEL_CONNSET.y1=SEL_CONNSET.y2=y;
			switch(i) {
				case 0: ptr = (u8*) &global_ipaddr; break;
				case 1: ptr = (u8*) &global_gateway; break;
				case 2: ptr = (u8*) &global_dns1; break;
				case 3: ptr = (u8*) &global_dns2; break;
				case 4: ptr = (u8*) &global_snmask; break;
			}
			snprintf(SEL_CONNSET.title,32,"%03i.%03i.%03i.%03i", ptr[0],ptr[1],ptr[2],ptr[3]);
			selectors[i] = connWindow->addSelector(&((const GUIData)SEL_CONNSET));
			y+=3;
		}	

		selectors[0]->setState(SEL_SELECTED);

		do {
			swiWaitForVBlank();
			mGUI->processInput();
		
			if(changeButton->getState()==BTN_ACTIVATED) {
				changeButton->setState(BTN_UP);
				for(i = 0; i < 5; i++) {
					if(selectors[i]->getState()==SEL_SELECTED) {
						selectors[i]->setState(SEL_ACTIVATED);
						i = 5;
					}
				}
			}
			for(i = 0; i < 5; i++) {
				if(selectors[i]->getState()==SEL_ACTIVATED) {
					selectors[i]->setState(SEL_UNSELECTED);
					sel = i;
				}
			}
			if (connWindow->getState()==WND_CANCEL || cancelButton->getState()==BTN_ACTIVATED) {
				mGUI->destroyWindow(connWindow);
				return false;
			} else if (connWindow->getState()==WND_OK  || OKButton->getState()==BTN_ACTIVATED) {
				mGUI->destroyWindow(connWindow);
				return true;
			}
		} while (sel < 0);
		
		mGUI->destroyWindow(connWindow);

		switch(sel) {
			case 0: enterIP (&global_ipaddr, "DS IP"); break;
			case 1: enterIP (&global_gateway, "Gateway Address"); break;
			case 2: enterIP (&global_dns1, "DNS Address 1"); break;
			case 3: enterIP (&global_dns2, "DNS Address 2"); break;
			case 4: enterIP (&global_snmask, "Subnet Mask"); break;
		}
	}
}

/*bool searchAP() {
	int numAP, i;
	Wifi_AccessPoint ap;
	
	CWindow *searchWind = mGUI->createWindow(&WND_SEARCHAP);
	CIconButton *cancelButton = searchWind->addIconButton(&BTN_SEARCHCANCEL);
	CButton *manualButton = searchWind->addButton(&BTN_SEARCHMANUAL);
	searchWind->addLabel(&LBL_SEARCHAP1);

	do {
		swiWaitForVBlank();
		
		mGUI->processInput();
		
		numAP = Wifi_GetNumAP();

		for (i = 0; i < numAP; i++) {
			if(Wifi_GetAPData(i,&ap)==WIFI_RETURN_OK) {
				if ((strcmp("",wifiData->ssid[0]) !=0 && strcmp (ap.ssid, wifiData->ssid[0])==0)) {
					readWFCData(0);
					mGUI->destroyWindow(searchWind);
					Wifi_GetAPData(i,&global_connectAP);
					return true;
				} else if (strcmp("",wifiData->ssid[1]) !=0 && strcmp (ap.ssid, wifiData->ssid[1])==0) {
					readWFCData(1);
					mGUI->destroyWindow(searchWind);
					Wifi_GetAPData(i,&global_connectAP);
					return true;
				} else if (strcmp("",wifiData->ssid[2]) !=0 && strcmp (ap.ssid, wifiData->ssid[2])==0) {
					readWFCData(2);
					mGUI->destroyWindow(searchWind);
					Wifi_GetAPData(i,&global_connectAP);
					return true;
				}
			}
		}

		if (searchWind->getState()==WND_CANCEL || cancelButton->getState()==BTN_ACTIVATED) {
			mGUI->destroyWindow(searchWind);
			return false;
		}
	} while (manualButton->getState()!=BTN_ACTIVATED);
	
	mGUI->destroyWindow(searchWind);
	return connectionMenu() && selectAP();
}*/

void startConnect(bool manual=false) {
	int i;
	char winTitle[32];

	CWindow *waitWindow;
	CLabel *statusLabel;
	CIconButton *cancelButton;

	if (manual) {
		if (connectionMenu() && selectAP()){
			Wifi_DisconnectAP();
//			Wifi_SetStaticIP(global_ipaddr,global_gateway,global_snmask,global_dns1,global_dns2);
			Wifi_ConnectAP(&global_connectAP,global_wepmode,global_wepkeyid,global_wepkeys[0]);
		}
		else {
			Wifi_DisconnectAP();
			Wifi_DisableWifi();
			return;
		}
	} else
		Wifi_AutoConnect();


	
	waitWindow = mGUI->createWindow(&WND_CONNWAIT);
	waitWindow->addLabel(&LBL_CONNWAIT);
	statusLabel = waitWindow->addLabel(&LBL_CONNSTATUS);
	cancelButton = waitWindow->addIconButton(&BTN_WAITCANCEL);

	snprintf (winTitle, 32, "Connecting..."); //, global_connectAP.ssid);
	waitWindow->setText(winTitle);
	
	do {
		swiWaitForVBlank();
		mGUI->processInput();

		statusLabel->setText((char*)assocstatus_strings[Wifi_AssocStatus()]);
		
		if (waitWindow->getState()==WND_CANCEL || cancelButton->getState()==BTN_ACTIVATED) {
			mGUI->destroyWindow(waitWindow);
			Wifi_DisconnectAP();
			Wifi_DisableWifi();
			return;
		}
	} while (Wifi_AssocStatus() != ASSOCSTATUS_ASSOCIATED);
	
	for (i = 30; i > 0; i--) swiWaitForVBlank();

	mGUI->destroyWindow(waitWindow);
	
	if(enterIP(&destip, "Server IP"))
		sendTouchUDP();

	Wifi_DisconnectAP();
	Wifi_DisableWifi();
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	powerON(POWER_ALL_2D); 
	fatInitDefault();
	irqInit();
	
	irqSet(IRQ_VBLANK, waitVbl);
	irqEnable(IRQ_VBLANK);
	
	videoSetMode(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE);
	vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, // VRAM_B_LCD
			VRAM_C_SUB_BG , VRAM_D_LCD); 

	SUB_BG0_CR = BG_COLOR_16 | BG_32x32 | BG_MAP_BASE(6) | BG_TILE_BASE(0);
	SUB_BG1_CR = BG_COLOR_16 | BG_32x32 | BG_MAP_BASE(7) | BG_TILE_BASE(0);
	SUB_BG3_CR = BG_BMP16_256x256;

	BG0_CR = BG_COLOR_16 | BG_32x32 | BG_MAP_BASE(6) | BG_TILE_BASE(0);
	BG1_CR = BG_COLOR_16 | BG_32x32 | BG_MAP_BASE(7) | BG_TILE_BASE(0);
	BG2_CR = BG_COLOR_16 | BG_32x32 | BG_MAP_BASE(5) | BG_TILE_BASE(1);
	//Screenshot
	BG3_CR = BG_BMP16_256x256;
		
//	BG_PALETTE_SUB[255] = RGB15(0,0,255);	//by default font will be rendered with color 255
	
	//consoleInit() is a lot more flexible but this gets you up and running quick
//	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(10), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);


	//these are rotation backgrounds so you must set the rotation attributes:
	//these are fixed point numbers with the low 8 bits the fractional part
	//this basicaly gives it a 1:1 translation in x and y so you get a nice flat bitmap
	BG3_XDX = 1 << 8;
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = 1 << 8;
	//our bitmap looks a bit better if we center it so scroll down (256 - 192) / 2 
	BG3_CX = 0;
	BG3_CY = 64 << 8;

	SUB_BG3_XDX = 1 << 8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1 << 8;
	SUB_BG3_CX = 0;
	SUB_BG3_CY = 64 << 8;
	
	lcdSwap();


	int i = 0;
	u16* vram = (u16*)(0x06020000);
	while(i++ <= (256*256)) vram[i] = 0;
	vram = (u16*)(0x06000000); i = 0;
	while(i++ <= (256*256)) vram[i] = 0;

	//Load the text I use into character slot 0 for both screens
	loadText((u16*)CHAR_BASE_BLOCK(0),(u16*)BG_PALETTE);
	loadText((u16*)CHAR_BASE_BLOCK_SUB(0),(u16*)BG_PALETTE_SUB);

	BG_PALETTE[0]=RGB(0,0,0);
	BG_PALETTE_SUB[0]=RGB(0,0,0);

	//*****WIFI*****//
	{ // send fifo message to initialize the arm7 wifi
		REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & clear FIFO
		
		u32 Wifi_pass= Wifi_Init(WIFIINIT_OPTION_USELED);
   	REG_IPC_FIFO_TX=0x12345678;
   	REG_IPC_FIFO_TX=Wifi_pass;
   	
		*((volatile u16 *)0x0400010E) = 0; // disable timer3
		
		irqSet(IRQ_TIMER3, Timer_50ms); // setup timer IRQ
		irqEnable(IRQ_TIMER3);
   	irqSet(IRQ_FIFO_NOT_EMPTY, arm9_fifo); // setup fifo IRQ
   	irqEnable(IRQ_FIFO_NOT_EMPTY);
   	
   	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ; // enable FIFO IRQ
   	
   	Wifi_SetSyncHandler(arm9_synctoarm7); // tell wifi lib to use our handler to notify arm7

		// set timer3
		*((volatile u16 *)0x0400010C) = -6553; // 6553.1 * 256 cycles = ~50ms;
		*((volatile u16 *)0x0400010E) = 0x00C2; // enable, irq, 1/256 clock
		
		while(Wifi_CheckInit()==0) { // wait for arm7 to be initted successfully
			while(VCOUNT>192); // wait for vblank
			while(VCOUNT<192);
		}
		
	} // wifi init complete - wifi lib can now be used!
	//***********//
	
	//Initialize the GUI for both screens, after this you only need to reference mGUI for the main screen and sGUI for the sub
	//parameteters: ((0)where to load the tiles, (1)where to use the tile map, (2)where text is loaded, (3)true if using the sub screen)
	sGUI = new CGUI((uint16*)(CHAR_BASE_BLOCK_SUB(0)+4096),(uint16*)SCREEN_BASE_BLOCK_SUB(7), (uint16*)SCREEN_BASE_BLOCK_SUB(6), true);
	mGUI = new CGUI((uint16*)(CHAR_BASE_BLOCK(0)+4096),(uint16*)SCREEN_BASE_BLOCK(7), (uint16*)SCREEN_BASE_BLOCK(6), false);
	
	initKeyboard();
//	showKeyboard(true);
	
	global_connectAP.ssid_len=16;
	strcpy(global_connectAP.ssid,"SpeedTouch2DDA10");
	global_connectAP.rssi=0;
	global_connectAP.channel=1;
	global_connectAP.flags=0;
	global_wepkeyid=0;
	global_wepmode=0;
	global_dhcp=0;
	global_ipaddr=0x3C01A8C0;
	global_snmask=0x00FFFFFF;
	global_gateway=0xFE01A8C0;
	global_dns1=0xFE01A8C0;
	global_dns2=0xFE01A8C0;
	
    FILE* fFile = fopen ("DS2Win.ini", "rb");
	char str[9];
	if (fFile) {
	  if (!feof(fFile)) {
	    fgets(str, 9, fFile);
	    destip = strtol(str,NULL,16);
		if (!feof(fFile))
	      for (i = 0; i < 10; i++) 
	        customKeys[i] = fgetc(fFile);	
		}
		fclose (fFile);
	  }
	else
	  destip = 0x4001A8C0;
	
	int sel;
	CWindow *mainWindow, *aboutWindow;
	CSelector *autoConnSel, *manualConnSel, *aboutSel;
	while (1) {
		swiWaitForVBlank();
		
		mainWindow = mGUI->createWindow(&WND_MAINMENU);
		autoConnSel = mainWindow->addSelector(&SEL_MAIN1);
		manualConnSel = mainWindow->addSelector(&SEL_MAIN2);
		aboutSel = mainWindow->addSelector(&SEL_MAIN3);

		autoConnSel->setState(SEL_SELECTED);
		
		sel = 0;

		do {
			swiWaitForVBlank();

			mGUI->processInput();

		} while (mainWindow->getState()!=WND_SEL);
		

		if(autoConnSel->getState()==SEL_ACTIVATED)
			sel = 1;
		else if(manualConnSel->getState()==SEL_ACTIVATED)
			sel = 2;
		else if(aboutSel->getState()==SEL_ACTIVATED)
			sel = 3;

		mGUI->destroyWindow(mainWindow);		//Don't forget to destroy the window if you use this GUI
		
		switch (sel) {
			case 1:
				startConnect();
			break;
			case 2:
				startConnect(true);
			break;
			case 3:
				aboutWindow = mGUI->createWindow(&WND_ABOUT);
				aboutWindow->addLabel(&LBL_ABOUT1);
				aboutWindow->addLabel(&LBL_ABOUT2);
				aboutWindow->addLabel(&LBL_ABOUT3);
				
				waitInput();

				mGUI->destroyWindow(aboutWindow);
			break;
		}

	}

	return 0;
}
