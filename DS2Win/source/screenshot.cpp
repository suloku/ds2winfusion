#include "screenshot.h"

CScreenShot::CScreenShot() {
	ready = true;
}

void CScreenShot::screenInit (u8 setNumBlocks) {
	int i;

	numBlocks = setNumBlocks;
	size = 0;
	curBlock = 0;
	ready = false;
	complete = false;
	retryDelay = 0;
	retryCount = 0;
	
	for (i = 0; i < numBlocks; i++)
		blocks[i]=false;
}

bool CScreenShot::screenWait () {
	if (retryDelay > 0) {
		retryDelay--;
		return false;
	} else
		return true;
}

void CScreenShot::screenCheckData () {
	int i;

	for (i = 0; i < numBlocks; i++) {
		if (!blocks[i]) {
			curBlock = i;
			retryDelay = RETRYDELAY;
			retryCount++;
			if (retryCount > 10)
				ready = true;
			return;
		}
	}
	return;
}

u8 CScreenShot::screenNextBlock () {
	int i = 0;

	for (i = curBlock; i < numBlocks; i++) {
		if (!blocks[i]) {
			curBlock = i+1;
			
			if (i >= numBlocks-1)
				screenCheckData();
			
			return i;
		}
	}
	
	screenCheckData();
	return curBlock;
}

void CScreenShot::screenFillData (u8 block, char* inBuf, int numBytes) {
	int i;
	
	if (blocks[block])
		return;

	for (i = 0; i < numBytes; i++)
        screenBuf[((block)*BLOCKSIZE)+i] = (u8) inBuf[i];
	
	size+=i;
	blocks[block]=true;

	
	for (i = 0; i < numBlocks; i++) {
		if (!blocks[i])
			return;
	}

	ready = true;
	complete = true;
}

void CScreenShot::displayScreen (u16* addr) {
	JPEG_DecompressImage(screenBuf, addr, 256, 192);
}

bool CScreenShot::screenIsReady () {return ready;}
bool CScreenShot::screenIsComplete () {return complete;}
void CScreenShot::setComplete (bool setComplete) { complete = setComplete; }
