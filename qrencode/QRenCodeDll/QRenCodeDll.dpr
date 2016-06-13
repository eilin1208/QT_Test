library QRenCodeDll;

{ Important note about DLL memory management: ShareMem must be the
  first unit in your library's USES clause AND your project's (select
  Project-View Source) USES clause if your DLL exports any procedures or
  functions that pass strings as parameters or function results. This
  applies to all strings passed to and from your DLL--even those that
  are nested in records and classes. ShareMem is the interface unit to
  the BORLNDMM.DLL shared memory manager, which must be deployed along
  with your DLL. To avoid using BORLNDMM.DLL, pass string information
  using PChar or ShortString parameters. }

uses
  SysUtils,
  Classes,
  Graphics,
  bitstream in '..\bitstream.pas',
  mask in '..\mask.pas',
  mmask in '..\mmask.pas',
  mqrspec in '..\mqrspec.pas',
  qrenc in '..\qrenc.pas',
  qrencode in '..\qrencode.pas',
  qrinput in '..\qrinput.pas',
  qrspec in '..\qrspec.pas',
  rscode in '..\rscode.pas',
  split in '..\split.pas',
  struct in '..\struct.pas';

{$R *.res}
var
  g_Margin: Integer=2;
  g_Size: Integer=3;
  g_EightBit: Integer=0;
  g_Casesens: Integer=1;
  g_Structured: Integer=0;
  g_Level: Integer=0;
  g_Code: Integer=0;
  g_Fore: TColor=clBlack;
  g_Back: TColor=clWhite;
  
//设置二维码参数
function QRCD_GenCfg(AMargin, ASize, AEightBit,ACasesens, AStructured, ALevel, ACode: Integer; nForeColor,nBackColor:Integer): Integer; stdcall;
begin
  g_Margin := AMargin;
  g_Size := ASize;
  g_EightBit := AEightBit;
  g_Casesens := ACasesens;
  g_Structured := AStructured;
  g_Level := ALevel;
  g_Code := ACode;
  g_Fore := TColor(nForeColor);
  g_Back := TColor(nBackColor);
end;

//二维生成
function QRCD_GenCode(const lpszText: PChar;const lpFileName:PChar): Boolean; stdcall;
var
  sText,sFileName:string;
begin
  sText := lpszText;
  sFileName:= lpFileName;
  try
    qr(
      sText,
      AnsiString(sFileName),
      g_Margin,
      g_Size,
      g_EightBit,
      g_Casesens,
      g_Structured,
      g_Level,
      g_Code,
      g_Fore,
      g_Back
    );
    Result := True;
  except
    on e:Exception do
    begin
      Result := False;
    end;
  end;
end;


exports
  QRCD_GenCfg,
  QRCD_GenCode; 

begin
end.
