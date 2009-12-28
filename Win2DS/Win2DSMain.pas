unit Win2DSMain;

interface

uses
  WinXP, Menus, LMDWndProcComponent, LMDTrayIcon, LMDCustomComponent,
  LMDOneInstance, ExtCtrls, IdBaseComponent, IdComponent, IdUDPBase,
  IdUDPServer, StdCtrls, Controls, ComCtrls, Classes, Forms, IdSocketHandle,
  func, keys, WinDSTypes, SysUtils, Math, Registry, Windows, IdGlobal;

type
  TWin2DSForm = class(TForm)
    out: TIdUDPServer;
    Log: TMemo;
    Timer1: TTimer;
    LMDOneInstance1: TLMDOneInstance;
    LMDTrayIcon1: TLMDTrayIcon;
    PopupMenu1: TPopupMenu;
    Restore1: TMenuItem;
    Exit1: TMenuItem;
    WinXP1: TWinXP;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    Password: TEdit;
    Alpha: TCheckBox;
    Minimize: TCheckBox;
    Label4: TLabel;
    GroupBox2: TGroupBox;
    Label3: TLabel;
    JPEGQuality: TTrackBar;
    Percent: TLabel;
    Quality: TComboBox;
    Label2: TLabel;
    GroupBox3: TGroupBox;
    Label5: TLabel;
    PercentZoom: TLabel;
    Label7: TLabel;
    JPEGQualityZoom: TTrackBar;
    QualityZoom: TComboBox;
    Startup: TCheckBox;
    Label6: TLabel;
    Frameskip: TEdit;
    PlayMovie: TButton;
    Path: TEdit;
    GetFolder: TButton;
    Label8: TLabel;
    NextMovie: TButton;
    procedure outUDPRead(AThread: TIdUDPListenerThread; AData: TIdBytes;
      ABinding: TIdSocketHandle);
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Timer1Timer(Sender: TObject);
    procedure Restore1Click(Sender: TObject);
    procedure Exit1Click(Sender: TObject);
    procedure FormMinimize(Sender: TObject);
    procedure JPEGQualityChange(Sender: TObject);
    procedure PlayMovieClick(Sender: TObject);
    procedure GetFolderClick(Sender: TObject);
    procedure NextMovieClick(Sender: TObject);
  private
    { Private declarations }
    procedure readRegistry ();
    procedure writeRegistry ();
  public
    { Public declarations }
  end;

var
  Win2DSForm: TWin2DSForm;
  PartStream, FullStream: TMemoryStream;
  cOut : array[1..4096] of char;
  lastMouseX, lastMouseY, frame, numFrames, lastPX, lastPY : integer;
  zoom : real;
  cPeerIP, aviFile : string;
  nPeerPort, nPings: integer;
  connected : boolean;
  OldFullCRC, OldPartCRC : WORD;
  cIn : string;

  procedure disconnectClient(reason: DISC_REASONS);

implementation

{$R *.dfm}

procedure TWin2DSForm.outUDPRead(AThread: TIdUDPListenerThread; AData: TIdBytes;
  ABinding: TIdSocketHandle);
var
  stream : TMemoryStream;
  size,x,y,px,py,i : integer;
  NewCRC : Word;
begin
  cIn := BytesToString(AData);
  case NET_COMMANDS(cIn[1]) of
    CONNECT: begin
             if StrComp(@cIn[2],PChar(Password.text)) = 0 then
               begin
               out.send(ABinding.PeerIP,ABinding.PeerPort,#0#1);
               Log.Lines.Append(ABinding.PeerIP+' Connected!');
               connected := true;
               cPeerIP   := ABinding.PeerIP;
               nPeerPort := ABinding.PeerPort;
               nPings := 0;
               Timer1.Enabled := false;
               Timer1.Interval := PINGDELAY;
               Timer1.Enabled := true;
               end
             else
               begin
               out.send(ABinding.PeerIP,ABinding.PeerPort,#0#0);
               Log.Lines.Append(ABinding.PeerIP+' Password Rejected!');
               end;
             end;
    end;

  if not connected then exit;

  case NET_COMMANDS(cIn[1]) of
		dsKEYPRESS:           begin
                          if PlayMovie.Caption = 'Stop Movie' then
                            case word(cIn[2]) of
                              CRR: frame := frame + 100;
                              CRL: if frame > 100 then frame := frame - 100;
                              CRU: if frameskip.text = '25' then frameskip.text := '1'
                                   else frameskip.text := inttostr(strtoint(frameskip.text)+1);
                              CRD: NextMovieClick(nil);
                              end
                          else
                            begin
            			          sendVirtualKey(word(cIn[2]), true);
                   		      sendVirtualKey(word(cIn[2]), false);
                            end;
            		          end;
		dsVKEYDOWN,dsKEYDOWN:	sendVirtualKey(word(cIn[2]), true);
		dsVKEYUP,dsKEYUP:    	sendVirtualKey(word(cIn[2]), false);
		MOUSECOORD: begin
          			x := ord(cIn[2])-2;
                y := ord(cIn[3])-2;

                if (x < 0) then x := 0;
                if (y < 0) then y := 0;

                px := (x*65535) div 253; // xlimit is 256 (255 on touchscreen)
                py := (y*65535) div 188; // ylimit is 192 (190 on touchscreen)
                setMouse(px, py, false);
                end;

		MOUSECOORDREL: begin
                   if (lastMouseX = 0) and (lastMouseY = 0) then
                     begin
                     lastMouseX := ord(cIn[2]);
                     lastMouseY := ord(cIn[3]);
                     end
                   else
                     begin
                     x := ord(cIn[2])-lastMouseX;
                     y := ord(cIn[3])-lastMouseY;

                     lastMouseX := ord(cIn[2]);
                     lastMouseY := ord(cIn[3]);
                     setMouse(x, y, true);
                     end;
                   end;
		MOUSECOORDRELSTART:
      begin
			lastMouseX := ord(cIn[2]);
			lastMouseY := ord(cIn[3]);
			end;
		MOUSECOORDZOOM:
      begin
			x := ord(cIn[2])-2;
			y := ord(cIn[3])-2;

			if (x < 0) then x := 0;
			if (y < 0) then y := 0;

      px := (screen.width*x)  div 253 - (trunc(256*zoom) div 2); // center at cursor
      py := (screen.height*y) div 188 - (trunc(192*zoom) div 2);
      if px < 0 then px := 0;
      if py < 0 then py := 0;
			if (px > (screen.width - trunc(256*zoom))) then px := (screen.width - trunc(256*zoom));  // make sure we arent showing past screen border
			if (py > (screen.height - trunc(192*zoom))) then py := (screen.height - trunc(192*zoom));

			px := (px*65535) div screen.width;  //  xlimit is 256 (255 on touchscreen)
			py := (py*65535) div screen.height;  //  ylimit is 192 (190 on touchscreen)
			px := px + (trunc(ord(cIn[4])*zoom*65535) div screen.width);
			py := py + (trunc(ord(cIn[5])*zoom*65535) div screen.height);

			setMouse (px, py, false);
			end;
		MOUSECLICKDOWN:  clickMouseDown();
		MOUSECLICKUP:    clickMouseUp();
		MOUSERCLICKDOWN: clickRightMouseDown();
		MOUSERCLICKUP:   clickRightMouseUp();
		MOUSEMCLICKDOWN: clickMiddleMouseDown();
		MOUSEMCLICKUP:   clickMiddleMouseUp();
    SCREENSHOT:
      begin
      if (numFrames > 0) then
        begin
        if (frame < numFrames) then
          begin
          frame := frame + strtoint(Frameskip.Text);
          CaptureAVI(frame, Quality.ItemIndex, JPEGQuality.Position,FullStream);
          end
        else if PlayMovie.Caption = 'Stop Movie' then NextMovieClick(nil);
        end
      else if (ord(cIn[2]) = 1) then
        begin
        if lastPY <= 0 then lastPY := 1;
        Capture(lastPX,lastPY-trunc(192*zoom),QualityZoom.ItemIndex,JPEGQualityZoom.Position,zoom,Alpha.Checked,FullStream)
        end
      else
        Capture(0,0,Quality.ItemIndex,JPEGQuality.Position,0,Alpha.Checked,FullStream);

      cOut[1] := cIn[1];                               // send same command back
      cOut[2] := chr(ceil(FullStream.Size/BLOCKSIZE)); // send number of blocks
      CalcCRC16(FullStream.Memory, FullStream.Size, NewCRC);
      if (OldFullCRC <> NewCRC) and (FullStream.Size < 32000) then
        out.send(ABinding.PeerIP,ABinding.PeerPort,copy(cOut,1,2));
      OldFullCRC := NewCRC;
      end;
    SCREENSHOTPART:
      begin
      x := ord(cIn[2])-2;
      y := ord(cIn[3])-2;
      zoom := ((ord(cIn[4])-1)/8)+1;

      if (x < 0) then x := 0;
      if (y < 0) then y := 0;

      px := (screen.width*x)  div 253 - (trunc(256*zoom) div 2); // center at cursor
      py := (screen.height*y) div 188 - (trunc(192*zoom) div 2);
      if px < 0 then px := 0;
      if py < 0 then py := 0;
      if (px > (screen.width - (256*zoom)))  then px := (screen.width - trunc(256*zoom)); // make sure we arent showing past screen border
      if (py > (screen.height - (192*zoom))) then py := (screen.height - trunc(192*zoom));

      Capture(px,py,QualityZoom.ItemIndex,JPEGQualityZoom.Position,zoom,Alpha.Checked,PartStream);

      lastPX := px; lastPY := py;

      cOut[1] := cIn[1];                               // send same command back
      cOut[2] := chr(ceil(PartStream.Size/BLOCKSIZE)); // send number of blocks
      CalcCRC16(PartStream.Memory, PartStream.Size, NewCRC);
      if (OldPartCRC <> NewCRC) and (PartStream.Size < 32000) then
        out.send(ABinding.PeerIP,ABinding.PeerPort,copy(cOut,1,2));
      OldPartCRC := NewCRC;
      end;
    SCREENSHOTPARTDATA,SCREENSHOTDATA:
      begin
      cOut[1] := cIn[1]; // send same command back
      cOut[2] := cIn[2]; // block number

      if NET_COMMANDS(cIn[1]) = SCREENSHOTDATA then stream := FullStream
      else stream := PartStream;

      stream.Seek(ord(cIn[2])*BLOCKSIZE, soFromBeginning);
      size := stream.Read(cOut[3], BLOCKSIZE);
      out.send(ABinding.PeerIP,ABinding.PeerPort,copy(cOut,1,size+2));
      end;
		PING:	nPings := 0;
		DISCONNECT:	disconnectClient(DISC_QUIT);
    end;

  while Log.Lines.Count > 200 do
    begin
    Log.Enabled := false;
    for i := 1 to 50 do
      Log.Lines.Delete(0);
    Log.SelStart := 999999999;
    Log.SelLength := 0;
    Log.Enabled := true;
    end;
end;

procedure TWin2DSForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  writeRegistry();
  if PlayMovie.Caption = 'Stop Movie' then PlayMovieClick(nil);
  disconnectClient(DISC_SHUTDOWN);
  out.active := false;
  FullStream.Free;
  PartStream.Free;
end;

procedure disconnectClient(reason: DISC_REASONS);
begin
	case reason of
		DISC_QUIT:    Win2DSForm.Log.Lines.Append('Client disconnected.');
		DISC_TIMEOUT: Win2DSForm.Log.Lines.Append('Client timed out.');
    end;
	connected := false;
  Win2DSForm.Timer1.Enabled := false;
end;

procedure TWin2DSForm.Timer1Timer(Sender: TObject);
begin
  if not connected then exit;

  out.send(cPeerIP,nPeerPort,chr(ord(PING)));
  inc(nPings);

  if (Timer1.Interval*nPings) > PINGTIMEOUT then
    disconnectClient (DISC_TIMEOUT);
end;

procedure TWin2DSForm.Restore1Click(Sender: TObject);
begin
  LMDTrayIcon1.Active := false;
  Win2DSForm.Show;
  Win2DSForm.BringToFront;
end;

procedure TWin2DSForm.Exit1Click(Sender: TObject);
begin
  close;
end;

procedure TWin2DSForm.FormMinimize(Sender: TObject);
begin
  LMDTrayIcon1.Active := true;
end;

procedure TWin2DSForm.FormCreate(Sender: TObject);
begin
  FullStream := TMemoryStream.Create;
  PartStream := TMemoryStream.Create;
  readRegistry ();
  frame := 0; numFrames := -1;
  Application.OnMinimize := FormMinimize;
  if Minimize.checked then FormMinimize(self);

  try
    out.Active := true;
    Log.Lines.Append('Server started - UDP port 8888');
  except on e:exception do
    Log.Lines.Append('Server could not attach to UDP port 8888 '+e.Message);
    end;
end;


procedure TWin2DSForm.readRegistry();
var
  Registry: TRegistry;
begin
  Registry := TRegistry.Create(KEY_READ);
  try
    Registry.RootKey := HKEY_LOCAL_MACHINE;
    if Registry.OpenKey('Software\Win2DS', False) then
      begin
      Password.text            := Encrypt(Registry.ReadString('Password'),false);
      Alpha.Checked            := Registry.ReadBool('CaptureAlpha');
      Minimize.Checked         := Registry.ReadBool('Minimize');
      Quality.ItemIndex        := Registry.ReadInteger('Quality');
      JPEGQuality.Position     := Registry.ReadInteger('JPEGQuality');
      QualityZoom.ItemIndex    := Registry.ReadInteger('QualityZoom');
      JPEGQualityZoom.Position := Registry.ReadInteger('JPEGQualityZoom');
      Path.text                := Registry.ReadString('MoviePath');
      Frameskip.text           := inttostr(Registry.ReadInteger('MovieFrameskip'));
      Registry.CloseKey;
      end;
    if Registry.OpenKey('Software\Microsoft\Windows\CurrentVersion\Run', False) then
      begin
      Startup.Checked := (Registry.ReadString('Win2DS') <> '');
      Registry.CloseKey;
      end;
  except
  end;

  Registry.Free;
end;

procedure TWin2DSForm.writeRegistry ();
var
  Registry: TRegistry;
begin
  Registry := TRegistry.Create();
  try
    Registry.RootKey := HKEY_LOCAL_MACHINE;
    // False because we do not want to create it if it doesn't exist
    if Registry.OpenKey('Software\Win2DS', true) then
      begin
      Registry.WriteString('Password',Encrypt(Password.text,true));
      Registry.WriteBool('CaptureAlpha',Alpha.Checked);
      Registry.WriteBool('Minimize',Minimize.Checked);
      Registry.WriteInteger('Quality',Quality.ItemIndex);
      Registry.WriteInteger('JPEGQuality',JPEGQuality.Position);
      Registry.WriteInteger('QualityZoom',QualityZoom.ItemIndex);
      Registry.WriteInteger('JPEGQualityZoom',JPEGQualityZoom.Position);
      Registry.WriteString('MoviePath',Path.text);
      Registry.WriteInteger('MovieFrameskip',strtoint(Frameskip.text));
      Registry.CloseKey;
      end;
    if Registry.OpenKey('Software\Microsoft\Windows\CurrentVersion\Run', True) then
      begin
      if Startup.Checked then
        Registry.WriteString('Win2DS','"' + ParamStr(0) + '"')
      else
        Registry.DeleteKey('Win2DS');
      Registry.CloseKey;
      end;
  except
  end;
  Registry.Free;
end;

procedure TWin2DSForm.JPEGQualityChange(Sender: TObject);
begin
  Percent.Caption := inttostr(JPEGQuality.Position)+'%';
  PercentZoom.Caption := inttostr(JPEGQualityZoom.Position)+'%';
end;

procedure TWin2DSForm.PlayMovieClick(Sender: TObject);
var
  sr: TSearchRec;
begin
  if PlayMovie.Caption = 'Stop Movie' then
    begin
    CloseAVI();
    PlayMovie.Caption := 'Start Movie';
    numFrames := -1;
    exit;
    end;

  frame := 0;
  numFrames := 0;

  if FindFirst(Path.text+'*.*', faAnyFile, sr) = 0 then
    begin
    repeat
      numFrames := LoadAVI(Path.text+sr.name);
    until (numFrames > 0) or (FindNext(sr) <> 0);

    if numFrames > 0 then
      begin
      aviFile := sr.Name;
      PlayMovie.Caption := 'Stop Movie';
      end
    else
      Log.Lines.Append('No playable movies could be found');

    SysUtils.FindClose(sr);
    end;
end;

procedure TWin2DSForm.GetFolderClick(Sender: TObject);
var
  cPath : string;
begin
  cPath := Path.Text;
  if GetFolderDialog(Application.Handle, 'Select a folder', cPath) then
    begin
    Path.Text := cPath + '\';
    writeRegistry();
    end;
end;

procedure TWin2DSForm.NextMovieClick(Sender: TObject);
var
  sr: TSearchRec;
  lFound: boolean;
begin
  if PlayMovie.Caption = 'Stop Movie' then PlayMovieClick(nil);
  frame := 0;
  numFrames := 0;
  lFound := (aviFile = '');

  if FindFirst(Path.text+'*.*', faAnyFile, sr) = 0 then
    begin
    repeat
    if lFound then
      numFrames := LoadAVI(Path.text+sr.name)
    else if sr.name = aviFile then
      lFound := true; // start searching from last found movie
    until (numFrames > 0) or (FindNext(sr) <> 0);

    aviFile := sr.Name;
    SysUtils.FindClose(sr);
    end;
  if numFrames > 0 then PlayMovie.Caption := 'Stop Movie'
  else PlayMovieClick(nil);
end;

end.
