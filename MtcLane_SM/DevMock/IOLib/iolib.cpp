#include "iolib.h"
IOCallBack g_call;
int length;
QByteArray Content;
Status datastatus;
QTcpClient* getTcpClient()
{
    static QTcpClient client;
    return &client;
}

int IO_Init(IOCallBack Func,int nType)
{
    if(Func == NULL)
        return -1;
    g_call = Func;
    getTcpClient()->SetType(DEV_IO);
    if(getTcpClient()->isConnected())
    {
        return 0;
    }
    else
    {
//        QString ip = "192.168.26.120";
        if(!QFile::exists("./IOLib"))
        {
            return -1;
        }
        QFile file("./IOLib");
        if(!file.open(QIODevice::ReadOnly))
        {
            return -1;
        }
        QString strValue = file.readAll();
        qDebug()<<strValue<<"  ip";
        file.close();
        getTcpClient()->SetFilename("./Logio.log");
        if(getTcpClient()->ConnecttoServer(strValue))
        {
            getTcpClient()->SetBack(callBack);
            return getTcpClient()->SendInit();;
        }
        return -1;
    }
}

void IO_SetAlarm(int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
      datastatus.bit_data.o = 1;
      datastatus.bit_data.p = 0;
    }
    else
    {
      datastatus.bit_data.o = 0;
      datastatus.bit_data.p = 1;
    }
    Content = QByteArray::number(datastatus.data);
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());

}
void IO_SetCanopy(int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
      datastatus.bit_data.j = 1;
      datastatus.bit_data.i = 0;
    }
    else
    {
      datastatus.bit_data.j = 0;
      datastatus.bit_data.i = 1;
    }
    Content = QByteArray::number(datastatus.data);
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());

}
void IO_SetBar(int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
      datastatus.bit_data.n = 0;
      datastatus.bit_data.m = 1;
    }
    else
    {
      datastatus.bit_data.n = 1;
      datastatus.bit_data.m = 0;
    }
    Content = QByteArray::number(datastatus.data);
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());
}
void IO_SetFogLamp(int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
      datastatus.bit_data.l = 1;
      datastatus.bit_data.k = 0;
    }
    else
    {
      datastatus.bit_data.l = 0;
      datastatus.bit_data.k = 1;
    }
    Content = QByteArray::number(datastatus.data);
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());
}
void IO_SetLaneLamp(int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
      datastatus.bit_data.l = 1;
      datastatus.bit_data.k = 0;
    }
    else
    {
      datastatus.bit_data.l = 0;
      datastatus.bit_data.k = 1;
    }
    Content = QByteArray::number(datastatus.data);
    length = Content.size();
    getTcpClient()->SendMessage(length,Content.data());
}
void IO_SetOut2 (int var)
{
    datastatus.data = 0;
    if(var == 0)
    {
        datastatus.bit_data.h = 0;
    }
    else
    {
        datastatus.bit_data.h = 1;
    }
}
void IO_Close()
{
   getTcpClient()->Disconnected();
}
void callBack(QByteArray &data)
{
    uint num;
    num = data.toUInt();
    (*g_call)(num);
}
