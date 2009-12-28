#include "nds.h"
#include <string.h>
#include <stdio.h>		//Remove after debugging done
#include "screenshot.h"
#include "dswifi9.h"
#include "sys\socket.h"
#include "netinet\in.h"
#include "netdata.h"
#include "screenshot.h"

int updateConnection();
void processScreenShot(CScreenShot* screenShot);
void processScreenShotPart(CScreenShot* screenShot);
void updateScreenShot(CScreenShot* screenShot, int delay, u16* vram, bool dualZm);
void updateScreenShotPart(CScreenShot* screenShot, int delay, u8 x, u8 y, u16* vram);
void setZoom(int zm);
void sendCommand (u8 command);
void sendMouseMove (u8 x, u8 y, bool relative);
void sendMouseMoveZoom (u8 startX, u8 startY, u8 x, u8 y);
void sendMouseRelStart (u8 x, u8 y);
void sendMouseZoomStart (u8 x, u8 y);
void sendKey (char key);
void sendKeyDown (char key);
void sendKeyUp (char key);
void sendVirtualKeyDown (char key);
void sendVirtualKeyUp (char key);
void sendBuf (int length);
bool openSocket (int port);
void sendPassword (char* password, unsigned long destip);
int isPassAccepted ();
void clientDisconnect();
