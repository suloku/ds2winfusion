unit WinDSTypes;

interface

const F_1	= $81;
const F_2	= $82;
const F_3	= $83;
const F_4	= $84;
const F_5	= $85;
const F_6	= $86;
const F_7	= $87;
const F_8	= $88;
const F_9	= $89;
const F10	= $8A;
const F11	= $8B;
const F12	= $8C;
const EXT	= $8D; // Exit
const HOM	= $8E; // Home
const PGU	= $8F; // Page Up
const PGD	= $90; // Page Down
const bEND = $91; // End
const TAB	= $92;
const ESC	= $93; // Escape
const BSP	= $80; // Backspace
const CAP	= $94; // Caps
const RET	= $A1; // Enter
const SHF	= $95; // Shift
const	CTL	= $96; // Ctrl
const SPC	= $20; // Space
const ALT	= $98; // Alt
const NDS	= $99; // DS
const SCN	= $9A; // Screen
const CRU	= $9B; // Cursor up
const CRD	= $9C; // Cursor down
const CRL	= $9D; // Cursor Left
const CRR	= $9E; // Cursor Right
const INS	= $9F; // Insert
const DEL	= $A0; // Delete
const MLC = $A2;
const MRC = $A3;
const MMC = $A4;
const ZOM = $A5;
const SWP = $A6;

const BLOCKSIZE	 = 1000;	//The size of blocks for screenshots. This shouldn't be changed.
const PINGDELAY	 = 1000;		//Delay between each ping.
const PINGTIMEOUT	= 60000;	//Time after not recieving pings to disconnect a client.

type
  NET_COMMANDS = (CONNECT, DISCONNECT, PING, MOUSECOORD, MOUSECOORDREL, MOUSECOORDRELSTART,
                  MOUSECOORDZOOM, MOUSECOORDZOOMSTART, MOUSECLICK, MOUSECLICKDOWN, MOUSECLICKUP,
                  MOUSERCLICK, MOUSERCLICKDOWN, MOUSERCLICKUP, MOUSEMCLICK, MOUSEMCLICKDOWN,
                  MOUSEMCLICKUP, dsKEYPRESS, dsKEYDOWN, dsKEYUP, dsVKEYDOWN, dsVKEYUP, SCREENSHOT, SCREENSHOTDATA,
                  SCREENSHOTPART, SCREENSHOTPARTDATA, FILESENDINIT, FILESENDINITRECEIVED, FILESENDDATA,
                  FILESENDDATARECEIVED);

type
  DISC_REASONS = (DISC_QUIT, DISC_TIMEOUT, DISC_SHUTDOWN);

implementation

end.
