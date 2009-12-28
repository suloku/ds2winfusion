#include "client.h"

int sock;
struct sockaddr_in sain, saout;
char outBuf[4096], rcvBuf[4096];
int screenDelay = 500, screenPartDelay = 50, rcvLen, zm;

int updateConnection() {
	//Screenshot Requests//
	rcvLen=recvfrom(sock,rcvBuf,4096,0,(struct sockaddr *)&sain,&rcvLen);
	
	if(rcvLen > 0) {
		switch ((uint8) rcvBuf[0]) {
			case PING:
				outBuf[0] = PING;
				sendBuf(1);
				break;
		}
	}

	return rcvLen;
}

void processScreenShot(CScreenShot* screenShot) {
	if(rcvLen > 0) {
		switch ((uint8) rcvBuf[0]) {
			case SCREENSHOT:
//				iprintf ("Screenshot init numBlocks: %i packet size:%i\n",rcvBuf[1],rcvLen);
				screenShot->screenInit (rcvBuf[1]);
				break;
			case SCREENSHOTDATA:
//				iprintf ("Screenshot block #%i of size %i recieved.\n", (u8) rcvBuf[1], rcvLen);
				screenShot->screenFillData (rcvBuf[1], &rcvBuf[2], rcvLen-2);
				break;
		}
	}
}

void processScreenShotPart(CScreenShot* screenShot) {
	if(rcvLen > 0) {
		switch ((uint8) rcvBuf[0]) {
			case SCREENSHOTPART:
//				iprintf ("Screenshot init numBlocks: %i packet size:%i\n",rcvBuf[1],rcvLen);
				screenShot->screenInit (rcvBuf[1]);
				break;
			case SCREENSHOTPARTDATA:
//				iprintf ("Screenshot block #%i of size %i recieved.\n", (u8) rcvBuf[1], rcvLen);
				screenShot->screenFillData (rcvBuf[1], &rcvBuf[2], rcvLen-2);
				break;
		}
	}
}

void updateScreenShot(CScreenShot* screenShot, int delay, u16* vram, bool dualZm) {
	u8 tmp;
	//Request Screen//
	screenDelay++;
	if ((screenDelay > delay) && screenShot->screenIsReady() && delay > 0) {
		//SendCommand (SCREENSHOT);
		outBuf[0] = SCREENSHOT;
		outBuf[1] = dualZm;
		sendBuf(2);
		screenDelay = 0;
//		iprintf ("Requested screenshot\n");
		
		if (screenShot->screenIsComplete())
			screenShot->displayScreen(vram);
	} else if (!screenShot->screenIsReady() && screenShot->screenWait()) { 
		tmp = screenShot->screenNextBlock();
//		iprintf ("Requested screen block %i\n", tmp);
		outBuf[0] = SCREENSHOTDATA;
		outBuf[1] = tmp;
		sendBuf(2);
	}
}

void updateScreenShotPart (CScreenShot* screenShot, int delay, u8 x, u8 y, u16* vram) {
	u8 tmp;
	
	//Request Screen//
	screenPartDelay++;
	if ((screenPartDelay > delay) && screenShot->screenIsReady() && delay > 0) {
		outBuf[0] = SCREENSHOTPART;
		outBuf[1] = x;
		outBuf[2] = y;
		outBuf[3] = zm;
		sendBuf(4);

		screenPartDelay = 0;
		
		if (screenShot->screenIsComplete())
			screenShot->displayScreen(vram);
	} else if (!screenShot->screenIsReady() && screenShot->screenWait()) { 
		tmp = screenShot->screenNextBlock();
		outBuf[0] = SCREENSHOTPARTDATA;
		outBuf[1] = tmp;
		sendBuf(2);
	}
}

void setZoom(int zoomLvl) {
	zm = zoomLvl;
}

void sendCommand (u8 command) {
	outBuf[0] = command;
	sendBuf(1);
}

void sendMouseMove (u8 x, u8 y, bool relative) {
	if(relative)
		outBuf[0] = (u8) MOUSECOORDREL;
	else
		outBuf[0] = (u8) MOUSECOORD;
	outBuf[1] = x;
	outBuf[2] = y;
	sendBuf(3);
}

void sendMouseMoveZoom (u8 startX, u8 startY, u8 x, u8 y) {
	outBuf[0] = (u8) MOUSECOORDZOOM;
	outBuf[1] = startX;
	outBuf[2] = startY;
	outBuf[3] = x;
	outBuf[4] = y;
	sendBuf(5);
}

void sendMouseRelStart (u8 x, u8 y) {
	outBuf[0] = MOUSECOORDRELSTART;
	outBuf[1] = x;
	outBuf[2] = y;
	sendBuf(3);
}

void sendMouseZoomStart (u8 x, u8 y) {
	outBuf[0] = MOUSECOORDZOOMSTART;
	outBuf[1] = x;
	outBuf[2] = y;
	sendBuf(3);
}

void sendKey (char key) {
	outBuf[0] = (u8) KEYPRESS;
	outBuf[1] = key;
	sendBuf(2);
}

void sendKeyDown (char key) {
	outBuf[0] = (u8) KEYDOWN;
	outBuf[1] = key;
	sendBuf(2);
}

void sendKeyUp (char key) {
	outBuf[0] = (u8) KEYUP;
	outBuf[1] = key;
	sendBuf(2);
}

void sendVirtualKeyDown (char key) {
	outBuf[0] = (u8) VKEYDOWN;
	outBuf[1] = key;
	sendBuf(2);
}

void sendVirtualKeyUp (char key) {
	outBuf[0] = (u8) VKEYUP;
	outBuf[1] = key;
	sendBuf(2);
}

void sendBuf (int length) {
	sendto(sock, outBuf, length,0,(struct sockaddr *)&saout,sizeof(saout));
}

bool openSocket (int port) {
	sock=socket(AF_INET,SOCK_DGRAM,0);

	saout.sin_family=sain.sin_family=AF_INET;
	saout.sin_port=sain.sin_port=htons(port);
	sain.sin_addr.s_addr=INADDR_ANY;
//	saout.sin_addr.s_addr=destip;

	bind(sock,(struct sockaddr *) &sain,sizeof(sain));
	
	int i = 1;
 	ioctl(sock, FIONBIO, &i);
	
	return true;
}

void sendPassword (char* password, unsigned long destip) {
	saout.sin_addr.s_addr=destip;

	outBuf[0]=CONNECT;
	strcpy(&outBuf[1], password);
	
	sendBuf(strlen(password)+2);

	sendto(sock, outBuf, strlen(password)+2 ,0,(struct sockaddr *)&saout,sizeof(saout));
}

int isPassAccepted () {
	int rcvLen;

	rcvLen=recvfrom(sock,rcvBuf,4096,0,(struct sockaddr *)&sain,&rcvLen);
	if (rcvLen > 0) {
		if (rcvBuf[0]==CONNECT) {
			if (rcvBuf[1] == 0)
				return 0;
			else if (rcvBuf[1] == 1)
				return 1;
		}
	}
	return -1;
}

void clientDisconnect() {
	sendCommand(DISCONNECT);

}
