#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>
#pragma pack(push,1)
//报文头
struct TMsgHeadInfo
{
    //报文长度，含报文头
    quint32 TotalLength;
    //报文编码
    char cMsgID[2];
    //报文类型
    char cMsgType[2];
    //压缩方式 '0'-不压缩 '1'-Zip压缩 '2'-rar压缩
    char ZipType;
    //发送方机构编码(6位站编码+3位车道编码)
    char Sender[9];
    //接收方机构编码
    char Receiver[9];
    //发送时间
    char SendTime[14];
    //MD5校验码
    char VerifyCode[32];
};

//授权信息
struct  TMsgAuthInfo
{
    //授权类型
    quint8 AuthType;
    //授权工号
    char AuthID[9];
    //授权姓名
    char AuthName[10];
    //授权时间
    char AuthTime[14];
};

// 基本回应报文
struct TMsgResponseInfo
{
    // 报文基本信息
    TMsgHeadInfo MsgHead;
    // 2.发送ID
    quint64 WasteID;
    // 处理结果(1:处理成功,2:处理失败,3:处理失败，需要重发)
    quint8 RecResult;
    //描述
    char Descirption[128];
    //授权信息
    TMsgAuthInfo AuthInfo;
};

//监控发送公告用的结构内容 wqs添加20081002
struct  TJKMsgInfo
{
    TMsgHeadInfo Header;
    //公告信息长度
    int MsgLength;
    //生效时间
    char UseTime[14];
    //失效时间
    char ExpireTime[14];
    //发送操作员
    char SenderOper[10];
    //优先级从1-9依次增高
    quint8 Priority;
    //字体
    char DisFont[20];
    //字符集 0西方 134汉字
    int CharSet;
    //字形 0 常规 1 粗体
    quint8 Style;
    //字体大小
    quint16 DisSize;
    //字体颜色
    int Color;
    //背景颜色
    int bkColor;
    //是否显示下划线 0常规 1下划线
    quint8 Underline;
    //显示方式0滚动 1 固定
    quint8 ShowWay;
    char MsgDetail[512];
};

#pragma pack(pop)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    curSocket(NULL), connectSocket(NULL)
{
    ui->setupUi(this);
    ui->plainTextEdit->appendPlainText(tr("begin listen 13334 %1").arg(tcpServer.listen(QHostAddress::Any, 7777)));
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newClient()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newClient()
{
    if(curSocket != NULL)
    {
        curSocket->close();
        delete curSocket;
        curSocket = NULL;
    }
    curSocket = tcpServer.nextPendingConnection();
    connect(curSocket, SIGNAL(readyRead()), this, SLOT(dataReceive()));
    ui->plainTextEdit->appendPlainText(
                tr("get new client ip:%1").arg(curSocket->peerAddress().toString()));

}

void MainWindow::dataReceive()
{
    ui->plainTextEdit->appendPlainText(
                tr("client send data ,length:%1\ncontent:%2")
                .arg(curSocket->bytesAvailable())
                .arg(curSocket->readAll().toHex().data()));
}

void MainWindow::on_pushButton_clicked()
{
    TMsgResponseInfo data;
    memset(&data, 0, sizeof(data));
    ui->plainTextEdit->appendPlainText(
                tr("write data to %1,length %2")
                .arg(curSocket->peerAddress().toString())
                .arg(curSocket->write((char*)&data, sizeof(data))));
}

void MainWindow::on_pushButton_2_clicked()
{
    TMsgResponseInfo data;
    memset(&data, 1, sizeof(data));
    ui->plainTextEdit->appendPlainText(
                tr("write data to %1,length %2")
                .arg(curSocket->peerAddress().toString())
                .arg(curSocket->write((char*)&data, sizeof(data))));
}

void MainWindow::on_pushButton_8_clicked()
{
    if(connectSocket != NULL)
    {
        connectSocket->close();
        delete connectSocket;
        connectSocket = NULL;
    }
    connectSocket = new QTcpSocket();
    connectSocket->connectToHost(ui->lineEdit_5->text(), 7711);
    bool ret = connectSocket->waitForConnected(1000);
    ui->plainTextEdit_2->appendPlainText(tr("connect to %1, %2").arg(ui->lineEdit_5->text()).arg(ret));
}

void MainWindow::on_pushButton_3_clicked()
{
    //报文头
    TMsgHeadInfo head;
    memset(&head, 0, sizeof(TMsgHeadInfo));
    memcpy(head.cMsgID, "11", 2);
    memcpy(head.cMsgType, "03", 2);
    head.ZipType = '1';
    memcpy(head.Sender, "004002021", 9);
    memcpy(head.Receiver, "004002021", 9);
    memcpy(head.SendTime, QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
    //报文体
    QByteArray data;
    data.append("01", 2);
    data.append("20", 2);
    data.append("00000000000000", 14);

    head.TotalLength = sizeof(head) + 18;
    memcpy(head.VerifyCode, QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex().data(), 32);
    data.prepend((char*)&head, sizeof(TMsgHeadInfo));

    connectSocket->write(data);
    ui->plainTextEdit_2->appendPlainText(data.data() + 4);
}

void MainWindow::on_pushButton_7_clicked()
{
    //报文头
    TMsgHeadInfo head;
    memset(&head, 0, sizeof(TMsgHeadInfo));
    memcpy(head.cMsgID, "11", 2);
    memcpy(head.cMsgType, "09", 2);
    head.ZipType = '1';
    memcpy(head.Sender, "004002021", 9);
    memcpy(head.Receiver, "004002021", 9);
    memcpy(head.SendTime, QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
    //报文体
    QByteArray data;
//    data.append("01", 2);
//    data.append("20", 2);
//    data.append("00000000000000", 14);

    head.TotalLength = sizeof(head);
    memcpy(head.VerifyCode, QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex().data(), 32);
    data.prepend((char*)&head, sizeof(TMsgHeadInfo));

    connectSocket->write(data);
    ui->plainTextEdit_2->appendPlainText(data.data() + 4);
}

void MainWindow::on_pushButton_6_clicked()
{
    //报文头
    TMsgHeadInfo head;
    memset(&head, 0, sizeof(TMsgHeadInfo));
    memcpy(head.cMsgID, "11", 2);
    memcpy(head.cMsgType, "08", 2);
    head.ZipType = '1';
    memcpy(head.Sender, "004002021", 9);
    memcpy(head.Receiver, "004002021", 9);
    memcpy(head.SendTime, QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
    //报文体
    QByteArray data;
    data.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);

    head.TotalLength = sizeof(head) + 14;
    memcpy(head.VerifyCode, QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex().data(), 32);
    data.prepend((char*)&head, sizeof(TMsgHeadInfo));

    connectSocket->write(data);
    ui->plainTextEdit_2->appendPlainText(data.data() + 4);
}

void MainWindow::on_pushButton_4_clicked()
{
}

void MainWindow::on_pushButton_5_clicked()
{

    TJKMsgInfo msg;
    memset(&msg, 0, sizeof(msg));
    TMsgHeadInfo &head = msg.Header;
    head.TotalLength = sizeof(TJKMsgInfo);
    memcpy(head.cMsgID, "11", 2);
    memcpy(head.cMsgType, "06", 2);
    head.ZipType = '1';
    memcpy(head.Sender, "004002021", 9);
    memcpy(head.Receiver, "004002021", 9);
    memcpy(head.SendTime, QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
    msg.MsgLength = 0;
    //生效时间
    memcpy(msg.UseTime, "20130308114011", 14);
    //失效时间
    memcpy(msg.ExpireTime, "20130308235911", 14);
    //发送操作员
    memcpy(msg.SenderOper, "operoper", 8);
    //优先级从1-9依次增高
    msg.Priority = 1;

    //字符集 0西方 134汉字
    msg.CharSet = 134;
    //字形 0 常规 1 粗体
    msg.Style = 0;
    //字体大小
    msg.DisSize = 12;
    //字体颜色
    msg.Color = 1;
//    //背景颜色
    msg.bkColor = 1;
//    int bkColor;
//    //是否显示下划线 0常规 1下划线
    msg.Underline = 0;
//    quint8 Underline;
//    //显示方式0滚动 1 固定
    msg.ShowWay = 0;
//    quint8 ShowWay;
    memcpy(msg.MsgDetail, "abcd123", 7);

    memcpy(msg.Header.VerifyCode,
           QCryptographicHash::hash(QByteArray(((char*)&msg) + sizeof(TMsgHeadInfo),
                                               sizeof(TJKMsgInfo) - sizeof(TMsgHeadInfo)),
                                    QCryptographicHash::Md5).toHex().data(),32);
    connectSocket->write((char*)&msg, sizeof(msg));
}
