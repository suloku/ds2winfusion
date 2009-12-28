//I designed the GUI class to use structs for data for windows and windowObjects
//to reduce clutter in the main files and so that there is a universally formatted
//input for windowObjects. There is probably a better way, but it works.

//Most are set to use parameters (x1,y1,x2,y2,title)
//You need to set x to the same for both if using a one-line windowObject

#include "gui.h"

WindowData WND_CUSTOMKEYS = {
	0, 2, 31, 21, (char*)"Setup Custom Keys"
};
const GUIData LBL_CUSTOM1 = {
	2, 17, 29, 17, "Use Y,X,A for mouse buttons."};
const GUIData LBL_CUSTOM2 = {
	2, 18, 29, 18, "Use L,R for Zoom-Set, Swap."};
const GUIData LBL_CUSTOM3 = {
	10, 19, 24, 19, "START to quit."};



WindowData WND_DESKTOP = {
	2, 0, 28, 23, (char*)"Virtual Desktop"
};
const IconButtonData BTN_TOPLEFT = {
	4, 4, 6, 6, "", ICON_LEFT };
const IconButtonData BTN_BOTTOMLEFT = {
	4, 7, 6, 9, "", ICON_LEFT };
const IconButtonData BTN_TOPRIGHT = {
	24, 4, 26, 6, "", ICON_RIGHT };
const IconButtonData BTN_BOTTOMRIGHT = {
	24, 7, 26, 9, "", ICON_RIGHT };
const GUIData LBL_TOPSPEED = {
	7, 5, 23, 5, ""};
const GUIData LBL_BOTTOMSPEED = {
	7, 8, 23, 8, ""};
const GUIData LBL_TOPLABEL = {
	9, 4, 21, 4, "Zoomed Screen:"};
const GUIData LBL_BTMLABEL = {
	10, 7, 20, 7, "Full Screen:"};
const GUIData GRP_KEYS = {
	4,11,26,15,"Controls" };
const GUIData RDO_KEYSRIGHT = {
	5,12,25,12,"Right-Handed" };
const GUIData RDO_KEYSLEFT = {
	5,13,25,13,"Left-Handed" };
const GUIData RDO_KEYSCUSTOM = {
	5,14,25,14,"Use Custom Keys" };
const GUIData CHK_MOUSEREL = {
	5, 17, 26, 17, "Relative Mouse" };
const GUIData BTN_DESKTOPQUIT = {
	5, 19, 15, 21, "Main Menu" };
const IconButtonData BTN_DESKTOPOK = {
	23, 19, 25, 21, "", BUTTON_OK };

const WindowData WND_GAMEPAD = {
	2, 2, 29, 19, (char*)"GamePad Mode"
};
const GUIData LBL_GAMEPAD1 = {
	4, 6, 27, 6, "This mode will use the" };
const GUIData LBL_GAMEPAD2 = {
	4, 7, 27, 7, "DS as a gamepad using" };
const GUIData LBL_GAMEPAD3 = {
	4, 8, 27, 8, "the custom keys and the" };
const GUIData LBL_GAMEPAD4 = {
	4, 9, 27, 9, "screen as a mouse. The" };
const GUIData LBL_GAMEPAD5 = {
	4, 10, 27, 10, "screens will turn off" };
const GUIData LBL_GAMEPAD6 = {
	4, 11, 27, 11, "to conserve energy." };
const GUIData LBL_GAMEPAD7 = {
	4, 13, 27, 13, "Press START+SEL to quit." };
const IconButtonData BTN_GAMEPADCANCEL = {
	12, 15, 14, 17, "", BUTTON_CANCEL };
const IconButtonData BTN_GAMEPADOK = {
	17, 15, 19, 17, "", BUTTON_OK };


const WindowData WND_CONNMAIN = {
	4, 4, 27, 18, (char*)"  Win2DS - Connected"
};
const GUIData SEL_DESKTOP = {
	6, 8, 25, 8, "Virtual Desktop" };
const GUIData SEL_GAMEPAD = {
	6, 10, 25, 10, "GamePad Mode" };
const GUIData SEL_CUSTOMKEYS = {
	6, 12, 25, 12, "Custom Keys" };
const GUIData SEL_DISCONNECT = {
	6, 14, 25, 14, "Disconnect" };
const GUIData SEL_RECONNECT = {
	6, 16, 25, 16, "Reconnect" };


const WindowData WND_PASSSTATUS = {
	5, 6, 26, 16, (char*)"Connection Status"
};
const GUIData LBL_PASSSTATUS = {
	8, 9, 24, 9, "Waiting..." };
const IconButtonData BTN_PASSCANCEL = {
	15, 12, 17, 14, "", BUTTON_CANCEL};


const WindowData WND_PASSWORD = {
	6, 7, 25, 13, (char*)"Enter Password"
};
const GUIData LBL_PASSWORD = {
	7, 11, 24, 11, ""};


const WindowData WND_CONNSETTINGS = {
	6, 0, 26, 23, (char*)"Connection Settings"
};
const GUIData GRP_SETIP = {
	8, 3, 24, 5, "DS IP" };
const GUIData GRP_SETGATEWAY = {
	8, 6, 24, 8, "Gateway Address" };
const GUIData GRP_SETDNS1 = {
	8, 9, 24, 11, "DNS Address" };
const GUIData GRP_SETDNS2 = {
	8, 12, 24, 14, "DNS Address 2" };
const GUIData GRP_SETSUBNET = {
	8, 15, 24, 17, "Subnet Mask" };
const IconButtonData BTN_CONNCANCEL = {
	8, 19, 10, 21, "", BUTTON_CANCEL };
const IconButtonData BTN_CONNOK = {
	22, 19, 24, 21, "", BUTTON_OK };
const GUIData BTN_CONNCHANGE = {
	12, 19, 20, 21, "Change" };



const WindowData WND_SELAP = {
	4,0,26,23,(char*)"Select your AP"
};
const GUIData LBL_NUMAP = {
	6, 3, 24, 3, "APs found: 0" };


const WindowData WND_SETIP = {
	5, 1, 25, 22,(char*)"Enter IP"
};
const IconButtonData BTN_SETIPCANCEL = {
	7, 18, 9, 20, "", BUTTON_CANCEL };
const IconButtonData BTN_SETIPOK = {
	21, 18, 23, 20, "", BUTTON_OK };


const WindowData WND_SEARCHAP = {
	4, 4, 28, 15, (char*)"Looking for your AP"};
const GUIData LBL_SEARCHAP1 = {
	6, 8, 25, 8, "Searching..."};
const IconButtonData BTN_SEARCHCANCEL = {
	6, 11, 8, 13, "", BUTTON_CANCEL };
const GUIData BTN_SEARCHMANUAL = {
	10, 11, 26, 13, "Manual Settings" };


const WindowData WND_CONNWAIT = {
	1, 7, 30, 13, (char*)"Connecting"
};
const GUIData LBL_CONNWAIT = {
	3, 11, 11, 11, "Status: " };
const GUIData LBL_CONNSTATUS = {
	11, 11, 26, 11, "" };
const IconButtonData BTN_WAITCANCEL = {
	26, 10, 28, 12, "", BUTTON_CANCEL};


/*const WindowData WND_CONNMENU = {
	3, 3, 28, 19, "Select your Connection"
};
const GUIData SEL_CONN1 = {
	5, 8, 26, 8, ""};
const GUIData SEL_CONN2 = {
	5, 11, 26, 11, ""};
const GUIData SEL_CONN3 = {
	5, 14, 26, 14, ""};
const GUIData SEL_CONN4 = {
	5, 16, 26, 16, "Manual Configure"};
const GUIData LBL_CONN1 = {
	10, 7, 21, 7, "WFC Data 1:"};
const GUIData LBL_CONN2 = {
	10, 10, 21, 10, "WFC Data 2:"};
const GUIData LBL_CONN3 = {
	10, 13, 21, 13, "WFC Data 3:"};*/



const WindowData WND_MAINMENU = {
	5, 6, 26, 18, (char*)"DS2Win Fusion 0.82a"
};
const GUIData SEL_MAIN1 = {
	6, 10, 25, 10, "Connect using WFC"
};                  
const GUIData SEL_MAIN2 = {
	6, 12, 25, 12, "Connect Manually"
};
const GUIData SEL_MAIN4 = {
	6, 14, 25, 14, "AutoPad Connect"
};
const GUIData SEL_MAIN3 = {
	6, 16, 25, 16, "About"
};



const WindowData WND_ABOUT = {
	1, 7, 30, 17, (char*)"About DS2Win"
};
const GUIData LBL_ABOUT1 = {
	3, 11, 28, 11, "DS2Win Fusion v0.82a"
};
const GUIData LBL_ABOUT2 = {
	3, 13, 28, 13, "Base Code: Bill Blaiklock"
};
const GUIData LBL_ABOUT3 = {
	3, 14, 28, 14, "Enhancements: Aaron Graham"
};
const GUIData LBL_ABOUT4 = {
	3, 15, 28, 15, "              Suloku"
};

