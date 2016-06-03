object frmMain: TfrmMain
  Left = 487
  Top = 48
  BorderStyle = bsSingle
  Caption = 'frmMain'
  ClientHeight = 414
  ClientWidth = 579
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object img1: TImage
    Left = 112
    Top = 16
    Width = 200
    Height = 200
    Stretch = True
  end
  object pnl1: TPanel
    Left = 0
    Top = 352
    Width = 579
    Height = 62
    Align = alBottom
    TabOrder = 0
    object btnGen: TButton
      Left = 480
      Top = 0
      Width = 75
      Height = 57
      Caption = #29983#25104
      TabOrder = 0
      OnClick = btnGenClick
    end
    object edtLog: TEdit
      Left = 8
      Top = 19
      Width = 457
      Height = 21
      TabOrder = 1
      Text = 
        '1234567890ABCDEF;'#31908'A12345;1;5;111;110;109;222;210;209;100000;2000' +
        '00;300000;1234500;2345600;12345678;'
    end
  end
end
