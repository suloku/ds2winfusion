unit func;

interface

uses VfW, Windows, SysUtils, Graphics, Classes, Jpeg, ShlObj;

procedure Capture(x, y, quality, JPEGQuality : integer;  zoom : real; captureAlpha:boolean; var JpegStream: TMemoryStream);
function  LoadAVI(avifn: string): integer;
procedure CaptureAVI(iFrameNumber, quality, JPEGQuality: Integer; var JpegStream: TMemoryStream);
procedure CloseAVI;
procedure DrawCursor(DC : HDC);
procedure BMPtoJPGStream(const Bitmap : TBitmap; var AStream: TMemoryStream; quality: integer);
procedure CalcCRC16 (p: pByte; nbyte: WORD; VAR CRCvalue: WORD);
function Encrypt(cPassword: string; lEncrypt: boolean): string;
function GetFolderDialog(Handle: Integer; Caption: string; var strFolder: string): Boolean;

CONST
  table: ARRAY[0..255] OF WORD =
 ($0000,$C0C1,$C181,$0140,$C301,$03C0,$0280,$C241,$C601,$06C0,$0780,
  $C741,$0500,$C5C1,$C481,$0440,$CC01,$0CC0,$0D80,$CD41,$0F00,$CFC1,
  $CE81,$0E40,$0A00,$CAC1,$CB81,$0B40,$C901,$09C0,$0880,$C841,$D801,
  $18C0,$1980,$D941,$1B00,$DBC1,$DA81,$1A40,$1E00,$DEC1,$DF81,$1F40,
  $DD01,$1DC0,$1C80,$DC41,$1400,$D4C1,$D581,$1540,$D701,$17C0,$1680,
  $D641,$D201,$12C0,$1380,$D341,$1100,$D1C1,$D081,$1040,$F001,$30C0,
  $3180,$F141,$3300,$F3C1,$F281,$3240,$3600,$F6C1,$F781,$3740,$F501,
  $35C0,$3480,$F441,$3C00,$FCC1,$FD81,$3D40,$FF01,$3FC0,$3E80,$FE41,
  $FA01,$3AC0,$3B80,$FB41,$3900,$F9C1,$F881,$3840,$2800,$E8C1,$E981,
  $2940,$EB01,$2BC0,$2A80,$EA41,$EE01,$2EC0,$2F80,$EF41,$2D00,$EDC1,
  $EC81,$2C40,$E401,$24C0,$2580,$E541,$2700,$E7C1,$E681,$2640,$2200,
  $E2C1,$E381,$2340,$E101,$21C0,$2080,$E041,$A001,$60C0,$6180,$A141,
  $6300,$A3C1,$A281,$6240,$6600,$A6C1,$A781,$6740,$A501,$65C0,$6480,
  $A441,$6C00,$ACC1,$AD81,$6D40,$AF01,$6FC0,$6E80,$AE41,$AA01,$6AC0,
  $6B80,$AB41,$6900,$A9C1,$A881,$6840,$7800,$B8C1,$B981,$7940,$BB01,
  $7BC0,$7A80,$BA41,$BE01,$7EC0,$7F80,$BF41,$7D00,$BDC1,$BC81,$7C40,
  $B401,$74C0,$7580,$B541,$7700,$B7C1,$B681,$7640,$7200,$B2C1,$B381,
  $7340,$B101,$71C0,$7080,$B041,$5000,$90C1,$9181,$5140,$9301,$53C0,
  $5280,$9241,$9601,$56C0,$5780,$9741,$5500,$95C1,$9481,$5440,$9C01,
  $5CC0,$5D80,$9D41,$5F00,$9FC1,$9E81,$5E40,$5A00,$9AC1,$9B81,$5B40,
  $9901,$59C0,$5880,$9841,$8801,$48C0,$4980,$8941,$4B00,$8BC1,$8A81,
  $4A40,$4E00,$8EC1,$8F81,$4F40,$8D01,$4DC0,$4C80,$8C41,$4400,$84C1,
  $8581,$4540,$8701,$47C0,$4680,$8641,$8201,$42C0,$4380,$8341,$4100,
  $81C1,$8081,$4040);

var
  hBmp: HBITMAP;
  pFile: PAVIFile;
  AVIStream: PAVIStream;
  gapgf: PGETFRAME;
  aviFilename: string;
  lastFrameNumber: integer;

implementation

function BrowseCallbackProc(hwnd: HWND; uMsg: UINT; lParam: LPARAM; lpData: LPARAM): Integer; stdcall;
begin
  if (uMsg = BFFM_INITIALIZED) then
    SendMessage(hwnd, BFFM_SETSELECTION, 1, lpData);
  BrowseCallbackProc := 0;
end;

function GetFolderDialog(Handle: Integer; Caption: string; var strFolder: string): Boolean;
var
  browse_info: TBrowseInfo;
  folder: array[0..MAX_PATH] of char;
  find_context: PItemIDList;
begin
  FillChar(browse_info,SizeOf(browse_info),#0);
  browse_info.pszDisplayName := @folder[0];
  browse_info.lpszTitle := PChar(Caption);
  browse_info.ulFlags := BIF_RETURNONLYFSDIRS;
  browse_info.hwndOwner := Handle;
  if strFolder <> '' then
    begin
    browse_info.lpfn := BrowseCallbackProc;
    browse_info.lParam := LongInt(PChar(strFolder));
    end;
  find_context := SHBrowseForFolder(browse_info);
  if Assigned(find_context) then
  begin
    if SHGetPathFromIDList(find_context,folder) then
      strfolder := folder
    else
      strfolder := '';
    GlobalFreePtr(find_context);
  end
  else
    strfolder := '';
end;

function Encrypt(cPassword: string; lEncrypt: boolean): string;
var
  i : integer;
begin
  result := '';
  for i := 1 to length(cPassword) do
    if lEncrypt then
      result := result + chr(ord(cPassword[i])+20+i)
    else
      result := result + chr(ord(cPassword[i])-20-i);
end;

procedure CalcCRC16 (p: pByte; nbyte: WORD; VAR CRCvalue: WORD);
var
    i: WORD;
    q: pByte;
begin
  CRCvalue := $FFFF;
  q := p;
  for i := 1 to nBYTE do
    begin
    CRCvalue := Hi(CRCvalue) XOR Table[ q^ XOR Lo(CRCvalue) ];
    INC(q)
    end;
  CRCvalue := not CRCvalue;
end;

procedure Capture(x, y, quality, JPEGQuality : integer;  zoom : real; captureAlpha:boolean; var JpegStream: TMemoryStream);
const
  CAPTUREBLT = $40000000; // Include alpha blended
var
  hdcScreen : HDC;
  hdcCompatible : HDC;
  hbmScreen : HBITMAP;
  bm, bmp : TBitmap;
begin
  hdcScreen     := CreateDC('DISPLAY', nil, nil, nil);
  hdcCompatible := CreateCompatibleDC(hdcScreen);
  hbmScreen     := CreateCompatibleBitmap(hdcScreen,GetDeviceCaps(hdcScreen, HORZRES),GetDeviceCaps(hdcScreen, VERTRES));
  SelectObject(hdcCompatible, hbmScreen);

  bmp := TBitmap.Create;
  bmp.Handle := hbmScreen;
  if captureAlpha then
    BitBlt(hdcCompatible,0,0,bmp.Width, bmp.Height,hdcScreen,0,0,SRCCOPY or CAPTUREBLT)
  else
    BitBlt(hdcCompatible,0,0,bmp.Width, bmp.Height,hdcScreen,0,0,SRCCOPY);
  DeleteDC(hdcScreen);
  DeleteDC(hdcCompatible);

  DrawCursor(bmp.Canvas.Handle);

  bm := TBitmap.Create;
  bm.Width := 256;
  bm.Height := 192;
  case quality of
   2:   SetStretchBltMode(bm.Canvas.Handle, BLACKONWHITE);
   1:   SetStretchBltMode(bm.Canvas.Handle, COLORONCOLOR);
   else SetStretchBltMode(bm.Canvas.Handle, HALFTONE);
   end;
  SetBrushOrgEx(bm.Canvas.Handle,0,0,nil);

  if zoom = 0 then
    StretchBlt(bm.Canvas.Handle, 0, 0, 256, 192, bmp.Canvas.Handle, 0, 0, bmp.Width, bmp.Height, SRCCOPY)
  else if zoom = 1 then
    BitBlt(bm.Canvas.Handle, 0, 0, 256, 192, bmp.Canvas.Handle, x, y, SRCCOPY)
  else
    StretchBlt(bm.Canvas.Handle, 0, 0, 256, 192, bmp.Canvas.Handle, x, y, trunc(256*zoom), trunc(192*zoom), SRCCOPY);

  bmp.FreeImage;
  FreeAndNil(bmp);

  JpegStream.Clear;
  BMPtoJPGStream(bm, JpegStream, JPEGQuality);
  bm.FreeImage;
  FreeAndNil(bm);
end;

procedure DrawCursor(DC : HDC);
var
  CI: TCursorInfo;
  Icon: TIcon;
  II: TIconInfo;
begin
  Icon := TIcon.Create;
  try
    CI.cbSize := SizeOf(CI);
    if GetCursorInfo(CI) then
      if CI.Flags = CURSOR_SHOWING then
        begin
        Icon.Handle := CopyIcon(CI.hCursor);
        if GetIconInfo(Icon.Handle, II) then
          begin
          DrawIcon (DC, ci.ptScreenPos.x - Integer(II.xHotspot), ci.ptScreenPos.y - Integer(II.yHotspot), Icon.Handle);
          DeleteObject(II.hbmMask);
          DeleteObject(II.hbmColor);
          end;
        end;
  finally
    Icon.Free;
  end;
end;

procedure BMPtoJPGStream(const Bitmap : TBitmap; var AStream: TMemoryStream; quality: integer);
var
  JpegImg: TJpegImage;
begin
   JpegImg := TJpegImage.Create;
   try
    JpegImg.CompressionQuality := quality;
    JpegImg.PixelFormat := jf24Bit;
    JpegImg.Assign(Bitmap);
    JpegImg.SaveToStream(AStream);
   finally
    JpegImg.Free
   end;
end;

function  LoadAVI(avifn: string): integer;
var
  Error: DWORD;
  AviInfo: TAVIFILEINFOW;
  sError: string;
begin
  Result := -1;
  // Initialize the AVIFile library.
  AVIFileInit;
  aviFilename := copy(avifn,LastDelimiter('\',avifn)+1,length(avifn));
  // The AVIFileOpen function opens an AVI file
  Error := AVIFileOpen(pFile, PChar(avifn), 0, nil);
  if Error <> 0 then
  begin
    AVIFileExit;
    case Error of
      AVIERR_BADFORMAT: sError := 'The file couldn''t be read';
      AVIERR_MEMORY: sError := 'The file could not be opened because of insufficient memory.';
      AVIERR_FILEREAD: sError := 'A disk error occurred while reading the file.';
      AVIERR_FILEOPEN: sError := 'A disk error occurred while opening the file.';
    end;
    //showmessage(serror);
    Result := -2;
    Exit;
  end;

  // AVIFileInfo obtains information about an AVI file
  if AVIFileInfo(pFile, @AVIINFO, SizeOf(AVIINFO)) <> AVIERR_OK then
  begin
    // Clean up and exit
    AVIFileRelease(pFile);
    AVIFileExit;
    Result := -3;
    Exit;
  end;

  // Show some information about the AVI
{  Form1.Log.Lines.Add('AVI Width : ' + IntToStr(AVIINFO.dwWidth));
  Form1.Log.Lines.Add('AVI Height : ' + IntToStr(AVIINFO.dwHeight));
  Form1.Log.Lines.Add('AVI Length : ' + IntToStr(AVIINFO.dwLength));}
  // Open a Stream from the file
  Error := AVIFileGetStream(pFile, AVIStream, streamtypeVIDEO, 0);
  if Error <> AVIERR_OK then
  begin
    // Clean up and exit
    AVIFileRelease(pFile);
    AVIFileExit;
    Result := -4;
    Exit;
  end;

  // Prepares to decompress video frames
  gapgf := AVIStreamGetFrameOpen(AVIStream, nil);
  if gapgf = nil then
  begin
    AVIStreamRelease(AVIStream);
    AVIFileRelease(pFile);
    AVIFileExit;
    Result := -5;
    Exit;
  end;

  // Show number of frames:
{  Form1.Log.Lines.Add(Format('Framstart: %d FrameEnd: %d',
    [AVIStreamStart(AVIStream), AVIStreamEnd(AVIStream)]));}
  result := AVIStreamEnd(AVIStream)-AVIStreamStart(AVIStream);//AVIINFO.dwLength;
end;

procedure CaptureAVI(iFrameNumber, quality, JPEGQuality: Integer; var JpegStream: TMemoryStream);
var
  TmpBmp, bm: TBitmap;
  DC_Handle: HDC;
  bits: PChar;
  lpbi: PBITMAPINFOHEADER;
begin
  iFrameNumber := iFrameNumber + AVIStreamStart(AVIStream);

  // Read current Frame
  // AVIStreamGetFrame Returns the address of a decompressed video frame
  lpbi := AVIStreamGetFrame(gapgf, iFrameNumber);
  if lpbi = nil then Exit;

  TmpBmp := TBitmap.Create;
  try
    TmpBmp.Height := lpbi.biHeight;
    TmpBmp.Width  := lpbi.biWidth;
    bits := Pointer(Integer(lpbi) + SizeOf(TBITMAPINFOHEADER));

    DC_Handle := CreateDC('Display', nil, nil, nil);
    try
      hBmp := CreateDIBitmap(DC_Handle, // handle of device context
        lpbi^, // address of bitmap size and format data
        CBM_INIT, // initialization flag
        bits, // address of initialization data
        PBITMAPINFO(lpbi)^, // address of bitmap color-format data
        DIB_RGB_COLORS); // color-data usage
    finally
      DeleteDC(DC_Handle);
    end;

    TmpBmp.Handle := hBmp;
//    if iFrameNumber < 200 then
    with tmpbmp.Canvas do
      begin
      Font.Name := 'Arial';
      Font.Size := TmpBmp.Height div 24;
      Font.Style := Font.Style + [fsBold];
      Pen.Width := 1;
      Brush.Style := bsClear;
      BeginPath(Handle);
      TextOut(2, TmpBmp.Height*14 div 15, aviFilename+' ['+inttostr(iFrameNumber-lastFrameNumber)+']');
      EndPath(Handle);
      Brush.Style := bsSolid;
      Brush.Color := clWhite;
      StrokeAndFillPath(Handle);
      end;

    try
      bm := TBitmap.Create;
      bm.Width := 256;
      bm.Height := 192;
      case quality of
       2: SetStretchBltMode(bm.Canvas.Handle, BLACKONWHITE);
       1: SetStretchBltMode(bm.Canvas.Handle, COLORONCOLOR);
       else SetStretchBltMode(bm.Canvas.Handle, HALFTONE);
       end;
      SetBrushOrgEx(bm.Canvas.Handle,0,0,nil);
      StretchBlt(bm.Canvas.Handle, 0, 0, 256, 192, TmpBmp.Canvas.Handle, 0, 0, TmpBmp.Width, TmpBmp.Height, SRCCOPY);

      JpegStream.Clear;
      BMPtoJPGStream(bm, JpegStream, JPEGQuality);
    except;
    end;
  finally
    TmpBmp.FreeImage;
    FreeAndNil(TmpBmp);
    bm.FreeImage;
    FreeAndNil(bm);
  end;
  lastFrameNumber := iFrameNumber;
end;

procedure CloseAVI;
begin
  if assigned(AVIStream) then
    begin
    AVIStreamGetFrameClose(gapgf);
    AVIStreamRelease(AVIStream);
    AVIFileRelease(pfile);
    AVIFileExit;
    end;
end;

end.
 