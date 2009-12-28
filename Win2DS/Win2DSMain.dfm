object Win2DSForm: TWin2DSForm
  Left = 264
  Top = 113
  Width = 670
  Height = 611
  Caption = 'Win2DS'
  Color = clBtnFace
  Constraints.MinHeight = 580
  Constraints.MinWidth = 670
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Arial'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  DesignSize = (
    662
    577)
  PixelsPerInch = 96
  TextHeight = 15
  object Log: TMemo
    Left = 5
    Top = 7
    Width = 332
    Height = 567
    Anchors = [akLeft, akTop, akRight, akBottom]
    Lines.Strings = (
      '-===============Win2DS 0.82===============-'
      ''
      '---------Originally written by Bill Blaiklock (sintax)---------'
      '---------Enhanced by Aaron Graham (monkeynz)---------'
      '')
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object GroupBox1: TGroupBox
    Left = 344
    Top = 8
    Width = 312
    Height = 566
    Anchors = [akTop, akRight, akBottom]
    Caption = 'Win2DS Control Panel'
    TabOrder = 1
    DesignSize = (
      312
      566)
    object Label1: TLabel
      Left = 24
      Top = 40
      Width = 56
      Height = 15
      Caption = 'Password'
    end
    object Label4: TLabel
      Left = 8
      Top = 545
      Width = 287
      Height = 15
      Anchors = [akLeft, akRight, akBottom]
      Caption = 'Settings will be applied instantly (no restart required)'
    end
    object Label6: TLabel
      Left = 8
      Top = 512
      Width = 59
      Height = 15
      Caption = 'Frameskip'
    end
    object Label8: TLabel
      Left = 8
      Top = 480
      Width = 69
      Height = 15
      Caption = 'Movie Folder'
    end
    object Password: TEdit
      Left = 152
      Top = 36
      Width = 121
      Height = 23
      PasswordChar = '*'
      TabOrder = 0
    end
    object Alpha: TCheckBox
      Left = 22
      Top = 72
      Width = 275
      Height = 25
      Caption = ' Capture Alpha-Blended (Transparent) Pixels'
      TabOrder = 1
    end
    object Minimize: TCheckBox
      Left = 22
      Top = 112
      Width = 115
      Height = 17
      Caption = ' Start Minimized'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
    object GroupBox2: TGroupBox
      Left = 16
      Top = 144
      Width = 281
      Height = 153
      Caption = 'Full Screen Settings'
      TabOrder = 3
      object Label3: TLabel
        Left = 16
        Top = 72
        Width = 242
        Height = 15
        Caption = 'JPEG Quality (lower quality = faster updates)'
      end
      object Percent: TLabel
        Left = 24
        Top = 120
        Width = 225
        Height = 15
        Alignment = taCenter
        AutoSize = False
        Caption = '100%'
      end
      object Label2: TLabel
        Left = 16
        Top = 32
        Width = 108
        Height = 15
        Caption = 'Resize Filter Quality'
      end
      object JPEGQuality: TTrackBar
        Left = 16
        Top = 88
        Width = 241
        Height = 33
        Max = 80
        Min = 1
        PageSize = 1
        Frequency = 5
        Position = 70
        TabOrder = 0
        TickMarks = tmTopLeft
        OnChange = JPEGQualityChange
      end
      object Quality: TComboBox
        Left = 136
        Top = 32
        Width = 137
        Height = 23
        ItemHeight = 15
        TabOrder = 1
        Text = 'High (Faster CPUs)'
        Items.Strings = (
          'High (Faster CPUs)'
          'Medium (Older CPUs)'
          'Low (Ancient CPUs)')
      end
    end
    object GroupBox3: TGroupBox
      Left = 16
      Top = 312
      Width = 281
      Height = 153
      Caption = 'Zoomed View Settings'
      TabOrder = 4
      object Label5: TLabel
        Left = 16
        Top = 72
        Width = 242
        Height = 15
        Caption = 'JPEG Quality (lower quality = faster updates)'
      end
      object PercentZoom: TLabel
        Left = 24
        Top = 120
        Width = 225
        Height = 15
        Alignment = taCenter
        AutoSize = False
        Caption = '100%'
      end
      object Label7: TLabel
        Left = 16
        Top = 32
        Width = 108
        Height = 15
        Caption = 'Resize Filter Quality'
      end
      object JPEGQualityZoom: TTrackBar
        Left = 16
        Top = 88
        Width = 241
        Height = 33
        Max = 80
        Min = 1
        PageSize = 1
        Frequency = 5
        Position = 70
        TabOrder = 0
        TickMarks = tmTopLeft
        OnChange = JPEGQualityChange
      end
      object QualityZoom: TComboBox
        Left = 136
        Top = 32
        Width = 137
        Height = 23
        ItemHeight = 15
        TabOrder = 1
        Text = 'High (Faster CPUs)'
        Items.Strings = (
          'High (Faster CPUs)'
          'Medium (Older CPUs)'
          'Low (Ancient CPUs)')
      end
    end
    object Startup: TCheckBox
      Left = 142
      Top = 112
      Width = 155
      Height = 17
      Caption = ' Start at Windows Startup'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object Frameskip: TEdit
      Left = 88
      Top = 512
      Width = 41
      Height = 23
      TabOrder = 6
      Text = '5'
    end
    object PlayMovie: TButton
      Left = 136
      Top = 512
      Width = 75
      Height = 25
      Caption = 'Start Movie'
      TabOrder = 7
      OnClick = PlayMovieClick
    end
    object Path: TEdit
      Left = 88
      Top = 480
      Width = 161
      Height = 23
      TabOrder = 8
    end
    object GetFolder: TButton
      Left = 256
      Top = 480
      Width = 35
      Height = 25
      Caption = '...'
      TabOrder = 9
      OnClick = GetFolderClick
    end
    object NextMovie: TButton
      Left = 216
      Top = 512
      Width = 75
      Height = 25
      Caption = 'Next Movie'
      TabOrder = 10
      OnClick = NextMovieClick
    end
  end
  object out: TIdUDPServer
    Bindings = <
      item
        IP = '0.0.0.0'
        Port = 8888
      end>
    DefaultPort = 8888
    OnUDPRead = outUDPRead
    Left = 16
    Top = 264
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 500
    OnTimer = Timer1Timer
    Left = 48
    Top = 264
  end
  object LMDOneInstance1: TLMDOneInstance
    Left = 80
    Top = 264
  end
  object LMDTrayIcon1: TLMDTrayIcon
    Active = False
    Icon.Data = {
      0000010001001010000001002000680400001600000028000000100000002000
      0000010020000000000040040000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000BCBCBC04BDBDBD04C8C8C802000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008C8C
      8C22353637CF272728D10E0E0FC70E0E0E9142424247C2C2C203000000000000
      0000000000000000000000000000000000000000000000000000000000008585
      8665AAAAABFEA3A4A5FE969799FE797A7CFE565758F72E2E2FC8161616904343
      43547A7A7A1FB6B6B606BEBEBE03000000000000000000000000000000008787
      8760DAD9D8FEC6C5C5FEB8B8B8FEA6A7A7FE9E9E9FFEA1A2A3FE9E9FA0FE8182
      83FE646566F73F4041D4212121CE0A0A0AAC2B2B2B619999990D000000008384
      845AD0CFCEFEA8A7A7FED0D0D0FE9A9B9CFEAD9A87FEA8A5A3FEA9AAAAFEABAB
      ABFEA7A7A8FEA7A7A8FEA3A9A5FEA4A5A6FE7C7E7EFD505152D98181811D8787
      8854A6A6A5FE454343FE9E9E9EFE9E958AFED77414FED67517FED3883BFED2A0
      69FECEAD89FEC4B6A7FECAC9C9FECFCFCDFEC3C3C2FE9E9E9EEE9E9E9E1A7D7D
      7D3EA9AAAAFD909090FEAAABABFEB59371FED77414FED87717FEE08D31FEE6A3
      4AFEDE9942FECEAA81FEDBDAD7FED1D0CCFEDFDEDBF985858445000000006A6A
      6A0D797A7AEC949495FE979898FE9F968CFEB59371FEC79461FED9A260FEE4B3
      68FEB3976FFED6D4D1FEB9B6B4FEB6B3B1FE7B7B7A6500000000000000007070
      7027C6C6C6FEC0C0C0FEB4B4B4FE717172FE939494FE9A9B9CFE909293FEA7A8
      A8FEA4A3A3FEAAA9A8FE9B9A99FD70706F850000000000000000000000007A7A
      7A3AC1C1C1FEAFB0B0FE8A8A8BFEB36111FEB26214FE9E5F20FE926233FE8969
      49FE989797FEAEAEAEFEA9A9A9FE7373733E0000000000000000000000008787
      874CBCBDBDFEADAEAEFE77706AFED57314FED87819FEDD882AFEDE892CFEB96D
      1FFE9F9F9FFEBBBBBBFE979696F4C5C5C5220000000000000000000000008D8D
      8D52B6B7B7FEA5A5A6FE6C6053FE955211FEB36A1EFED69743FEE6A84FFE9873
      47FEB3B3B4FEC3C3C3FE72727266A2A2A1AEF5F5F50B00000000000000006969
      69020D0D0D2D1C1C1C46292929623131327F3030319E363736BD4E4A44DD7572
      6FFAABACACFE969697E7B4B4B409E9E9E920F3F3F30F00000000000000000000
      000000000000000000000000000000000000000000000000000000000000B5B5
      B504BCBCBC22C4C4C41E00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      00008FFF000001FF0000000F0000000100000000000000000000000100000003
      00000007000000070000000700000003000000030000FF1F0000FFFF0000}
    PopupMenu = PopupMenu1
    ThreadPriority = tpIdle
    OnDblClick = Restore1Click
    Left = 112
    Top = 264
  end
  object PopupMenu1: TPopupMenu
    MenuAnimation = [maBottomToTop]
    Left = 144
    Top = 264
    object Restore1: TMenuItem
      Caption = '&Restore'
      OnClick = Restore1Click
    end
    object Exit1: TMenuItem
      Caption = '&Exit'
      OnClick = Exit1Click
    end
  end
  object WinXP1: TWinXP
    Left = 176
    Top = 264
  end
end
