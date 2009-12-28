#pragma once

enum NET_COMMANDS {
	CONNECT,
	DISCONNECT,
	PING,
	MOUSECOORD,
	MOUSECOORDREL,
	MOUSECOORDRELSTART,
	MOUSECOORDZOOM,
	MOUSECOORDZOOMSTART,
	MOUSECLICK,
	MOUSECLICKDOWN,
	MOUSECLICKUP,
	MOUSERCLICK,
	MOUSERCLICKDOWN,
	MOUSERCLICKUP,
	MOUSEMCLICK,
	MOUSEMCLICKDOWN,
	MOUSEMCLICKUP,
	KEYPRESS,
	KEYDOWN,
	KEYUP,
	VKEYDOWN,
	VKEYUP,
	SCREENSHOT,
	SCREENSHOTDATA,
	SCREENSHOTPART,
	SCREENSHOTPARTDATA,
	FILESENDINIT,
	FILESENDINITRECIEVED,
	FILESENDDATA,
	FILESENDDATARECIEVED
};


#define BLOCKSIZE	1000	//The size of blocks for screenshots. This shouldn't be changed.
#define RETRYDELAY	15		//Delay to retry after not recieving a screenblock.
#define PINGDELAY	500		//Delay between each ping.
#define PINGTIMEOUT	3000	//Time after not recieving pings to disconnect a client.

#define MAXRETRIES	2		//Number of times to retry getting a screen block before giving up.
							//This was designed so that it could be used over an unstable connection, but
							//I don't think it works well. There is also a bug I was never able to fix
							//That retries the last block indefinately, so this is set low.

