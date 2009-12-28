program Win2DS;

uses
  Forms,
  Win2DSMain in 'Win2DSMain.pas' {Win2DSForm},
  func in 'func.pas',
  WinDSTypes in 'WinDSTypes.pas',
  keys in 'keys.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'Win2DS';
  Application.CreateForm(TWin2DSForm, Win2DSForm);
  Application.Run;
end.
