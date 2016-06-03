unit MainForm;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls;

type
  QRE_Init = procedure();stdcall;
  QRE_SetQRString = procedure(sMsg:PChar; iLen : Integer);stdcall;
  QRE_genQrencode  = function(sImageFileName:PChar):Integer;stdcall;
  TfrmMain = class(TForm)
    img1: TImage;
    pnl1: TPanel;
    btnGen: TButton;
    edtLog: TEdit;
    procedure FormShow(Sender: TObject);
    procedure btnGenClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

  Init : QRE_Init = nil;
  DecryptPaperStr : QRE_SetQRString=nil;
  DecryptQrencode : QRE_genQrencode=nil;

implementation
uses
  Jpeg;

{$R *.dfm}

procedure TfrmMain.FormShow(Sender: TObject);
var
  g_hCardLibModule: HMODULE;
begin
  g_hCardLibModule := LoadLibrary(PChar('.\QREncryptDll.dll'));
  Init := QRE_Init(GetProcAddress(g_hCardLibModule, 'QRE_Init'));
  DecryptPaperStr := QRE_SetQRString(GetProcAddress(g_hCardLibModule,'QRE_SetQRString'));
  DecryptQrencode := QRE_genQrencode(GetProcAddress(g_hCardLibModule,'QRE_genQrencode'));
  Init();
end;

procedure TfrmMain.btnGenClick(Sender: TObject);
var
  buffer:array[0..1024] of char;
  tmpstr : String;
begin
  tmpstr := edtLog.Text;
  FillChar(buffer, SizeOf(buffer), 0);
  Move(tmpstr[1], buffer[0], Length(tmpstr));
  DecryptPaperStr(buffer, Length(tmpstr));
  DecryptQrencode('./af.jpg');

  img1.Picture.LoadFromFile('af.jpg');
end;

end.
