#include "QREncryptDll.h"
#include "tcQrencode.h"
#include <QObject>
#include <QTextCodec>
#include "ilogmsg.h"

ILogMsg g_logMsg;
QString g_sMsg;

QString Utf8toGB2312(const QString& str)
{
    return QTextCodec::codecForName("GB2312")->fromUnicode(str);
}

QString GB2312toUtf8(const char *str, int size=-1)
{
    if(size == -1)
    {
        return QTextCodec::codecForName("GB18030")->toUnicode(str);
    }
    else
    {
        return QTextCodec::codecForName("GB18030")->toUnicode(str, size);
    }
}

void QRE_Init()
{
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    g_logMsg.loadConfig("://QREncrypt.properties");
}

void QRE_SetQRString(const char *sMsg, int nLen)
{
    g_sMsg = GB2312toUtf8(sMsg, nLen);
}

int QRE_genQrencode(const char *sImageFileName)
{
    LogFmtMsg()->info(QObject::tr("%1").arg(g_sMsg));

    TcQrencode t;
    QImage image;
    image = t.encodeImage(g_sMsg);
    image.save(sImageFileName);

    return 0;
}
