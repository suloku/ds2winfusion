#pragma once

#include "nds.h"
#include "netdata.h"
#include "gba-jpeg-decode.h"

#define SCREENVRAM			(u16*)(0x06008000)
#define SCREENPARTVRAM		(u16*)(0x06208000)
#define SCREENSHOTDELAY 100
#define RETRYCOUNT 10

class CScreenShot
{
private:
	u8 numBlocks;
	u8 curBlock;
	u16 size;
	bool ready, complete;
	unsigned char screenBuf[32768];
	bool blocks[32];
	int retryDelay;
	int retryCount;
public:
	CScreenShot ();
	void screenInit (u8 setNumBlocks);
	u8 screenNextBlock ();
	void screenFillData (u8 block, char* inBuf, int numBytes);
	bool screenIsReady ();
	bool screenIsComplete ();
	void displayScreen (u16* addr);
	void screenCheckData ();
	bool screenWait ();
	void setComplete (bool setComplete);
};
