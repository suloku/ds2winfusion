unit keys;

interface

uses types;

procedure sendVirtualKey (key: WORD; down: boolean);
procedure sendKey(key: WORD; virtualKey, down: boolean);
procedure setMouse(x, y: integer; relative: boolean);
procedure sendMouseInput(flag: DWORD);
procedure clickMouseDown();
procedure clickMouseUp();
procedure clickRightMouseDown();
procedure clickRightMouseUp();
procedure clickMiddleMouseDown();
procedure clickMiddleMouseUp();

implementation

uses Windows, WinDSTypes;

procedure sendVirtualKey (key: WORD; down: boolean);
begin
	case key of
		F_1: sendKey(VK_F1, true, down);
		F_2: sendKey(VK_F2, true, down);
		F_3: sendKey(VK_F3, true, down); 
		F_4: sendKey(VK_F4, true, down); 
    F_5: sendKey(VK_F5, true, down);
		F_6: sendKey(VK_F6, true, down);
		F_7: sendKey(VK_F7, true, down); 
		F_8: sendKey(VK_F8, true, down); 
		F_9: sendKey(VK_F9, true, down); 
		F10: sendKey(VK_F10, true, down); 
		F11: sendKey(VK_F11, true, down); 
		F12: sendKey(VK_F12, true, down); 
		HOM: sendKey(VK_HOME, true, down);
		PGU: sendKey(VK_PRIOR, true, down);
		PGD:sendKey(VK_NEXT, true, down);
		bEND: sendKey(VK_END, true, down);
		TAB: sendKey(VK_TAB, true, down);
		ESC: sendKey(VK_ESCAPE, true, down);
		BSP: sendKey(VK_BACK, true, down);
		RET: sendKey(VK_RETURN, true, down);
		CRU: sendKey(VK_UP, true, down);
		CRD: sendKey(VK_DOWN, true, down);
		CRL: sendKey(VK_LEFT, true, down);
		CRR: sendKey(VK_RIGHT, true, down);
		INS: sendKey(VK_INSERT, true, down);
		DEL: sendKey(VK_DELETE, true, down);
		SCN,NDS,ALT,CTL,SHF,CAP,EXT:;
    44..57,59,61,91..93,
		96..122: sendKey(VkKeyScan(char(key)), true, down);
    else sendKey(key, false, down);

{  if down then keybd_event(VkKeyScan(char(key)), MapVirtualKey(key,0), 0, 0)
  else keybd_event(VkKeyScan(char(key)), MapVirtualKey(key,0), KEYEVENTF_KEYUP, 0);  }

    end;
end;

procedure sendKey(key: WORD; virtualKey, down: boolean);
const
  KEYEVENTF_UNICODE = $0004;
var
	keyStroke: TKEYBDINPUT;
  out: TINPUT;
begin
	if virtualKey then
    begin
		keyStroke.wVk := key;
		keyStroke.wScan := 0;
		if down then keyStroke.dwFlags := 0
		else    		 keyStroke.dwFlags := KEYEVENTF_KEYUP;
    end
	else
    begin
		keyStroke.wVk := 0;
		keyStroke.wScan := key;
		if down then keyStroke.dwFlags := KEYEVENTF_UNICODE
		else  	   	 keyStroke.dwFlags := KEYEVENTF_UNICODE or KEYEVENTF_KEYUP;
    end;

	keyStroke.time := 0;
	keyStroke.dwExtraInfo := 0;

  out.Itype := INPUT_KEYBOARD;
  out.ki := keystroke;
	SendInput (1, out, sizeof (out));
//	SendInput (1, keyStroke, sizeof (keyStroke));
//	debugPrint ("%s pressed %i",virtualKey ? "virtualkey" : "key", key);
end;

procedure setMouse (x, y: integer; relative: boolean);
var
	moveMouse: TMOUSEINPUT;
	outInput: TInput;
begin
	moveMouse.dx := x;
	moveMouse.dy := y;
	moveMouse.mouseData := 0;
	if (relative) then
		moveMouse.dwFlags := MOUSEEVENTF_MOVE
	else
		moveMouse.dwFlags := MOUSEEVENTF_ABSOLUTE or MOUSEEVENTF_MOVE;
	moveMouse.time := 0;
	moveMouse.dwExtraInfo := 0;

	outInput.Itype := INPUT_MOUSE;
	outInput.mi := moveMouse;

	SendInput (1, outInput, sizeof (outInput));
end;

procedure sendMouseInput (flag: DWORD);
var
	mouseDown: TMOUSEINPUT;
	outInput: TINPUT;
begin
	mouseDown.dx := 0;
	mouseDown.dy := 0;
	mouseDown.mouseData := 0;
	mouseDown.dwFlags := flag;
	mouseDown.time := 0;
	mouseDown.dwExtraInfo := 0;

	outInput.Itype := INPUT_MOUSE;
	outInput.mi := mouseDown;

	SendInput (1, outInput, sizeof (outInput));
end;

procedure clickMouseDown();       begin sendMouseInput(MOUSEEVENTF_LEFTDOWN);   end;
procedure clickMouseUp();         begin sendMouseInput(MOUSEEVENTF_LEFTUP);     end;
procedure clickRightMouseDown();  begin sendMouseInput(MOUSEEVENTF_RIGHTDOWN);  end;
procedure clickRightMouseUp();    begin sendMouseInput(MOUSEEVENTF_RIGHTUP);    end;
procedure clickMiddleMouseDown(); begin sendMouseInput(MOUSEEVENTF_MIDDLEDOWN); end;
procedure clickMiddleMouseUp();   begin sendMouseInput(MOUSEEVENTF_MIDDLEUP);   end;

end.

