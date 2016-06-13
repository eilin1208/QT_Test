{*******************************************************************************

 *
 * Copyright (C) 2014 Hao Shi <admin@hicpp.com> 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
                                                               
    revision history
      2014-04-21

*******************************************************************************}

unit frmuQRcode;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls {$IF CompilerVersion>=23.0},System.UITypes{$IFEND};

type
  TWndProc = function(AHwnd: HWND; AMsg: UINT; AWParam: WPARAM;
    ALParam: LPARAM): LRESULT; stdcall;

  TfrmQRcode = class(TForm)
    btnGen: TButton;
    edtMargin: TEdit;
    lbl1: TLabel;
    edtSize: TEdit;
    lbl2: TLabel;
    lbl3: TLabel;
    cbbLevel: TComboBox;
    lbl4: TLabel;
    cbbCasesens: TComboBox;
    lbl5: TLabel;
    cbbStructured: TComboBox;
    lbl6: TLabel;
    cbbEightBit: TComboBox;
    lbl7: TLabel;
    lbl8: TLabel;
    clrbxFore: TColorBox;
    clrbxBack: TColorBox;
    lbl9: TLabel;
    lbl10: TLabel;
    cbbCode: TComboBox;
    edtOutput: TEdit;
    procedure btnGenClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    FEdit: THandle;
    FNewProc: Pointer;
    FOldProc: Pointer;

//    FOutput: THandle;
//    FOutputNewProc: Pointer;
//    FOutputOldProc: Pointer;
    { Private declarations }
    procedure CreateEdit();
    procedure WMCommand(var Message: TWMCommand); message WM_COMMAND;
    procedure EditProc(var Msg: TMessage);
//    procedure OutputProc(var Msg: TMessage);
  public
    { Public declarations }
  end;

var
  frmQRcode: TfrmQRcode;

implementation

{$R *.dfm}
{$R WindowsXP}

uses
  qrenc;

const
  ID_EDIT = 10001;
  ID_OUTPUT = 10002;

procedure TfrmQRcode.btnGenClick(Sender: TObject);
var
  sText, sPath: WideString;
  iLen: Integer;
begin
  iLen := GetWindowTextLengthW(FEdit);
  if iLen > 0 then
  begin
    SetLength(sText, iLen);
    GetWindowTextW(FEdit, PWideChar(sText), iLen);
  end;
//  iLen := GetWindowTextLengthW(FOutput);
//  if iLen > 0 then
//  begin
//    SetLength(sPath, iLen);
//    GetWindowTextW(FOutput, PWideChar(sPath), iLen);
//  end;
  try
    qr(
      sText,
      AnsiString(edtOutput.Text),
//      sPath,
      StrtoIntDef(edtMargin.Text, 2),
      StrToIntDef(edtSize.Text, 3),
      cbbEightBit.ItemIndex,
      cbbCasesens.ItemIndex,
      cbbStructured.ItemIndex,
      cbbLevel.ItemIndex,
      cbbCode.ItemIndex,
      clrbxFore.Selected,
      clrbxBack.Selected
    );
    MessageDlg('生成成功！', mtInformation, [mbOK], -1);
  except
    MessageDlg('生成失败！', mtInformation, [mbOK], -1);
  end;
end;

procedure TfrmQRcode.CreateEdit;
begin
//  FOutputNewProc := Classes.MakeObjectInstance(OutputProc);
//  FOutput := CreateWindowEx(WS_EX_CLIENTEDGE, 'Edit', '', WS_CHILD or WS_VISIBLE
//    or WS_CLIPSIBLINGS or ES_LEFT or WS_TABSTOP, 
//    69, 108, 308, 20, Handle,
//    HMENU(ID_OUTPUT), HInstance, nil);
//  SendMessage(FOutput, WM_SETFONT, Font.Handle, 0);
//  FOutputOldProc := Pointer(SetWindowLong(
//    FOutput, GWL_WNDPROC, Integer(FOutputNewProc)));  

  FNewProc := Classes.MakeObjectInstance(EditProc);
  FEdit := CreateWindowEx(WS_EX_CLIENTEDGE, 'Edit', '', WS_CHILD or WS_VISIBLE
    or WS_CLIPSIBLINGS or ES_LEFT or ES_MULTILINE or ES_WANTRETURN
    or WS_VSCROLL or ES_AUTOVSCROLL or WS_TABSTOP, 8, 140, 430, 130, Handle,
    HMENU(ID_EDIT), HInstance, nil);
  SendMessage(FEdit, WM_SETFONT, Font.Handle, 0);
  FOldProc := Pointer(SetWindowLong(FEdit, GWL_WNDPROC, Integer(FNewProc)));
end;

procedure TfrmQRcode.EditProc(var Msg: TMessage);
var
  dc: HDC;
  ps: TPaintStruct;
  lpRect: TRect;
begin
  if Msg.Msg = WM_PAINT then
  begin
    BeginPaint(FEdit, ps);
    try
      dc := ps.hdc;
      GetClipBox(dc, lpRect);
      FillRect(dc, lpRect, GetStockObject(WHITE_BRUSH));
      Msg.Result := 1;
      Exit;
    finally
      EndPaint(FEdit, ps);
    end;
  end;
  Msg.Result := CallWindowProc(FOldProc, FEdit, Msg.Msg, Msg.WParam, Msg.LParam);
end;

procedure TfrmQRcode.FormCreate(Sender: TObject);
begin    
  CreateEdit();
//  SetWindowTextW(FOutput, PWideChar(ExtractFilePath(ParamStr(0)) + '1.bmp'));
  clrbxFore.Selected := clBlack;
  clrbxBack.Selected := clWhite;
  cbbStructured.ItemIndex := 0;
  cbbLevel.ItemIndex := 0;
  cbbCasesens.ItemIndex := 1;
  cbbEightBit.ItemIndex := 0;
  cbbCode.ItemIndex := 0;
end;

procedure TfrmQRcode.WMCommand(var Message: TWMCommand);
begin
  if Message.ItemID = ID_EDIT then
  begin
    OutputDebugString('123123');
  end;
  if Message.ItemID = ID_OUTPUT then
  begin
  
  end;
  inherited;
end;

procedure TfrmQRcode.FormDestroy(Sender: TObject);
begin
  SetWindowLong(FEdit, GWL_WNDPROC, Integer(FOldProc));
  Classes.FreeObjectInstance(FNewProc);
  DestroyWindow(FEdit);

//  SetWindowLong(FOutput, GWL_WNDPROC, Integer(FOutputOldProc));
//  Classes.FreeObjectInstance(FOutputNewProc);
//  DestroyWindow(FOutput);
end;

//procedure TfrmQRcode.OutputProc(var Msg: TMessage);
//var
//  dc: HDC;
//  ps: TPaintStruct;
//  lpRect: TRect;
//begin
//  if Msg.Msg = WM_PAINT then
//  begin
//    BeginPaint(FOutput, ps);
//    try
//      dc := ps.hdc;
//      GetClipBox(dc, lpRect);
//      FillRect(dc, lpRect, GetStockObject(WHITE_BRUSH));
//      Msg.Result := 1;
//      Exit;
//    finally
//      EndPaint(FOutput, ps);
//    end;
//  end;
//  Msg.Result := CallWindowProc(FOutputOldProc, FOutput, Msg.Msg, Msg.WParam,
//    Msg.LParam);
//end;

end.
