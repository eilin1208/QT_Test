#include "laneutils_new.h"
#include "LaneType.h"
#ifdef Q_OS_UNIX
#include <sys/vfs.h>
#include <sys/sysinfo.h>
#else
#include <windows.h>
#include <time.h>
#endif
quint32 crctable[256] =
{
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};
int ExtractComId(const char *szConnStr)
{
    size_t nLen=strlen(szConnStr);
    if (0==nLen)
    {
        return 0;
    }
    else if (3>=nLen)
    {
        // 假设全部是数字
        return atoi(szConnStr);
    }
    else
    {
        // 假设前3个字符为"COM"
        return atoi(szConnStr+3);
    }
}

bool TransToStdComPortName(char *szDest, size_t nMaxLen, const char *szSource)
{
    int nPort=ExtractComId(szSource);
    if (nPort<=0)
    {
        return false;
    }
    int nCharNumStored;
    QString strComName;
    if (nPort<10)
    {
        strComName = QString("COM%1").arg(nPort);
        nCharNumStored=strComName.length();

    }
    else
    {
        // To specify a COM port number greater than 9, use the following syntax: "\\.\COM10".
        strComName = QString("\\\\.\\COM%1").arg(nPort);
        nCharNumStored=strComName.length();

    }
    return (nCharNumStored>0);
}

void SetBit(quint32 *pdwBase, int nIndex, bool bStatus)
{
    // 负数表示采用相反的逻辑
    if (nIndex<0)
    {
        nIndex=-nIndex;
        bStatus=!bStatus;
    }
    // 最多只处理32位，从0开始，超出范围忽略
    if (0==nIndex || nIndex>32) return;
    int n=1;
    if (nIndex>1)
    {
        n<<=(nIndex-1);
    }
    quint32 dwNewStatus=*pdwBase;
    if (bStatus)
    {
        dwNewStatus=(dwNewStatus | n);
    }
    else
    {
        dwNewStatus=(dwNewStatus & (~n));
    }
    *pdwBase=dwNewStatus;
}

bool GetBitStatus(const quint32 dwBase, int nIndex)
{
    int i;
    // 负数表示采用相反的逻辑
    if (nIndex<0)
    {
        i=-nIndex;
    }
    else
    {
        i=nIndex;
    }
    // 最多只处理32位，从0开始，超出范围返回false
    if (0==i || i>32) return false;
    int n=1;
    if (i>1)
    {
        n<<=(i-1);
    }
    if (0==(dwBase & n))
    {
        return (nIndex>0)?false:true;
    }
    else
    {
        return (nIndex>0)?true:false;
    }
}
/********************************************************************
Description:
将整数转化为指定长度的字符数组
Parameters:
szBuf	字符数组指针
nBufLen	字符数组长度
ul		要转换的无符号长整数
Return Values:
0		成功
-1		参数错误
>0		转换成功，但余数未转换完，有溢出
*********************************************************************/
int ulong2StrBuf(char *szBuf, size_t nBufLen, unsigned long ul)
{
    if (nBufLen<1) return -1;
    for (int i = (int)nBufLen-1; i>=0; i--)
    {
        szBuf[i] = char(ul % 10 + '0');
        ul /= 10;
    }
    return ul;
}

// 类似于ulong2StrBuf
quint64 ulonglong2DecStr(char *szBuf, size_t nBufLen, quint64 ull)
{
    if (nBufLen<=0) return -1;
    for (int i = (int)nBufLen-1; i>=0; i--)
    {
        szBuf[i] = char(ull % 10 + '0');
        ull /= 10;
    }
    return int (ull);
}

/********************************************************************
Description:
将有可能不以0字符结尾的十进制字符串转换成整数
Parameters:
szBuf	字符数组指针
nBufLen	字符数组长度，应大于0，小于10。
Return Values:
转换后的整数
*********************************************************************/
unsigned long StrBuf2Ulong(const char *szBuf, size_t nBufLen)
{
    char buf[32];
    if ((nBufLen<1) || (nBufLen>16))
    {
        return 0;
    }
    memcpy(buf, szBuf, nBufLen);
    buf[nBufLen]=0;
    return atol(buf);
}

char *FormatTime(const QDateTime &SysTime)
{
    static char buf[32];
    strcpy(buf, SysTime.toString("yyyy/MM/dd hh:mm:ss").toLocal8Bit().data());
    return buf;
}

void FormatTime(const QDateTime &SysTime, char *szTime)
{
    char buf[32];
    // 格式为"YYYY/MM/DD HH:NN:SS"的字符串指针
    strcpy(buf, SysTime.toString("yyyy/MM/dd hh:mm:ss").toLocal8Bit().data());
    // 注意：这样szTime并不一定是0字符结尾
    memcpy(szTime, buf, 20);
}

char *FormatTimeToChar14(const QDateTime &SysTime)
{
    static char buf[32];
    memset(buf,0,sizeof(buf));
    strcpy(buf, SysTime.toString("yyyyMMddhhmmss").toLocal8Bit().data());
    return buf;
}

void FormatTimeToChar14(const QDateTime &SysTime,char *sTime14)
{
    if(SysTime.isValid())
    {
        memcpy(sTime14, SysTime.toString("yyyyMMddhhmmss").toLocal8Bit().data(), 14);
    }
    return;
}

void DateTimeToChar14(const QDateTime& dateTime, char *sTime14)
{
    FormatTimeToChar14(dateTime, sTime14);
}

void DateTimeToChar14(const QTSYSTEMTIME& sysTime, char *sTime14)
{
    QDateTime datetime;
    SysTime2QDateTime(&sysTime, datetime);
    FormatTimeToChar14(datetime, sTime14);
}

#define BaseDate1970 11644473600
#define BaseDate2000 12591158400
#define BaseDate197001010800 (BaseDate1970+8*3600)


int GetDbcsCharNum(const char *szBuf)
{
    int n=0;
    bool bLeadDbcsLeadByte=FALSE;
    for (int i=0;i<(int)strlen(szBuf);i++)
    {
        if (szBuf[i]>0)
        {
            bLeadDbcsLeadByte=FALSE;
            n++;
        }
        else if (bLeadDbcsLeadByte)
        {
            bLeadDbcsLeadByte=FALSE;
            n++;
        }
        else
        {
            bLeadDbcsLeadByte=TRUE;
        }
    }
    return n;
}

inline int Char2Hex(char c)
{
    if ('0'<=c && c<='9') return c-'0';
    if ('a'<=c && c<='f') return c-'a'+10;
    if ('A'<=c && c<='F') return c-'A'+10;
    return -1;
}

inline char Hex2Char(quint8 b)
{
    if (b<10) return b+'0';
    if (b<16) return b-10+'a';
    return '0';
}

bool Hex2Raw(quint8 *pData, const char *szMsg, int nMsgLen)
{
    int hi, lo;
    for (int i=0;i<nMsgLen/2;i++)
    {
        hi=Char2Hex(szMsg[i*2]);
        lo=Char2Hex(szMsg[i*2+1]);
        if (hi<0 || lo<0) return false;
        pData[i]=(hi<<4)+lo;
    }
    return true;
}



quint32 bcd2dword(const char *bcd, int len)
{
    quint32 u=0;
    for (int i=0; i<len; i++)
    {
        if (0xFF==(quint8)(bcd[i]))
        {
            break;
        }
        if (0x0F==(bcd[i]&0x0F))
        {
            u=bcd2byte(bcd[i])+u*10;
            break;
        }
        u=bcd2byte(bcd[i])+u*100;
    }
    return u;
}

quint64 bcd2dword64(const char *bcd, int len)
{
    quint64 u=0;
    for (int i=0; i<len; i++)
    {
        if (0xFF==(quint8)(bcd[i]))
        {
            break;
        }
        if (0x0F==(bcd[i]&0x0F))
        {
            u=bcd2byte(bcd[i])+u*10;
            break;
        }
        u=bcd2byte(bcd[i])+u*100;
    }
    return u;
}

bool ll2bcd(char *bcd, int bcd_len, quint64 d64)
{
    int tmp = 0;
    for(int i = bcd_len - 1; i >= 0; i--)
    {
        tmp = d64 % 100;
        bcd[i] = ((tmp / 10) << 4) + ((tmp % 10) & 0x0f);
        d64 /= 100;
    }
    return true;
}

bool bcd2systime(QDateTime *pTime, const char *bcd, int nBcdLen)
{
    pTime->setDate(QDate(
                       (quint16)bcd2dword(bcd, 2),
                       bcd2byte(bcd[2]),
                       bcd2byte(bcd[3])));
    if (nBcdLen>=7)
    {
        pTime->setTime(QTime(
                           bcd2byte(bcd[4]),
                           bcd2byte(bcd[5]),
                           bcd2byte(bcd[6])));
    }

    return pTime->isValid();
}

bool systime2bcd(char *bcd, int len, const QDateTime &SysTime)
{
    if ((len<2) || (len>7))
    {
        return FALSE;
    }
    char buf[64];

    strcpy(buf, SysTime.toString("yyyyMMddhhmmss").toLocal8Bit().data());
    Hex2Raw((quint8 *)bcd, buf, min(len*2, 14));
    return TRUE;
}

void Raw2HexStr(char *pStr, const quint8 *pRaw, int nRawLen)
{
    quint8 b;
    for (int i=0; i<nRawLen; i++)
    {
        b=pRaw[i];
        pStr[2*i]=Hex2Char(b>>4);
        pStr[2*i+1]=Hex2Char(b & 0x0f);
    }
    pStr[2*nRawLen]=0;
}

char *HexStr(const quint8 *pBuf, int nBufLen)
{
    const int MAX_MSG_LEN=256;
    static char s[MAX_MSG_LEN];
    if (nBufLen<0 || nBufLen*2>=MAX_MSG_LEN)
    {
        s[0]=0;
    }
    else
    {
        Raw2HexStr(s, pBuf, nBufLen);
    }
    return s;
}

int ArCharToInt(const char *szBuff, int nBuffLen)
{
    if ((NULL==szBuff) || (nBuffLen<=0))
    {
        return 0;
    }
    char tmpBuff[11];
    memset(tmpBuff, 0, sizeof(tmpBuff));
    memcpy(tmpBuff, szBuff, min(nBuffLen, (int)sizeof(tmpBuff)-1));
    return atoi(tmpBuff);
}
bool ConvertChar14ToSystemTime(QTSYSTEMTIME *pSysTime, const char *szTime)
{
    QDateTime qTmpTime;
    ConvertChar14ToSystemTime(&qTmpTime, szTime);
    QDateTime2SysTime(qTmpTime, pSysTime);
    return true;
}
bool ConvertChar14ToSystemTime(QDateTime *pSysTime, const char *szTime)
{
    if (NULL==szTime)
    {
        return FALSE;
    }
    pSysTime->setDate(QDate(
                          ArCharToInt(szTime,4),
                          ArCharToInt(szTime+4,2),
                          ArCharToInt(szTime+6,2)
                          ));
    pSysTime->setTime(QTime(
                          ArCharToInt(szTime+8,2),
                          ArCharToInt(szTime+10,2),
                          ArCharToInt(szTime+12,2)));

    return pSysTime->isValid();
}

bool IsValidIP(const char *szIP)
{
    QHostAddress tempAddress(szIP);
    if(!tempAddress.isNull())
    {
        QString ip(szIP);
        QStringList ipList = ip.split(".");
        if(ipList.at(0).compare("0") == 0 || ipList.at(3).compare("0") == 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;

}

bool DirectoryExists(const char *szPathName)
{
    return QDir(szPathName).exists();
}
bool FileExists(const char *szFileName)
{
    return QFile(szFileName).exists();
}

bool MoveFileReplaceExisting(QString lpExistingFileName, QString lpNewFileName)
{
    QFile sourceFile(lpExistingFileName);
    QFile destFile(lpNewFileName);
    sourceFile.open(QFile::ReadOnly);
    destFile.open(QFile::WriteOnly);

    QDataStream sourceStream(&sourceFile);
    QDataStream destStream(&destFile);
    char *buf = new char[sourceFile.size()];
    int readCount = sourceStream.readRawData(buf, sourceFile.size());
    int writeCount = destStream.writeRawData(buf, sourceFile.size());
    delete [] buf;
    if(readCount != writeCount)
    {
        return false;
    }
    destFile.close();
    sourceFile.close();
    return sourceFile.remove();
}

bool MoveFilesInDirectory(QString lpSourceDirectory, QString lpTargetDirectory, bool bIncludeChildrenFolder/*=false*/)
{
    QDir fromDir(lpSourceDirectory);
    QDir toDir(lpTargetDirectory);

    if(toDir.exists() == false)
    {
        if(!toDir.mkdir(toDir.absolutePath()))
        {
            return false;
        }
    }

    QFileInfoList fileInfoList = fromDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName().compare(".") == 0 || fileInfo.fileName().compare("..") == 0)
        {
            continue;
        }

        //拷贝子目录
        if(fileInfo.isDir())
        {
            if(bIncludeChildrenFolder == false)
            {
                continue;
            }
            if(!MoveFilesInDirectory(fileInfo.filePath(), toDir.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
        else
        {
            if(!MoveFileReplaceExisting(fileInfo.filePath(), toDir.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
    }

    return true;
}

bool WriteBufToFile(QString lpImageName, const char * lpBuffer, quint32 nBufferLen)
{
    QFile destFile(lpImageName);
    if(!destFile.open(QFile::WriteOnly))
    {
        return false;
    }

    QDataStream destStream(&destFile);
    int rlt = destStream.writeRawData(lpBuffer, nBufferLen);
    if(rlt != nBufferLen)
    {
        return false;
    }
    destFile.close();
    return true;
}

bool ReadBufFromFile(QByteArray & lpBuffer, quint32 &nBufferLen, QString & lpImageName)
{
    QFile destFile(lpImageName);
    if(!destFile.open(QFile::ReadOnly))
    {
        return false;
    }
    lpBuffer = destFile.read(nBufferLen);
    return lpBuffer.length() == nBufferLen;
}

bool ForceDirectories(const char *szPathName)
{
    QDir tempDir;
    return tempDir.mkpath(szPathName);
}
quint8 Swap4Bits(quint8 b )
{
    quint8 High = (b<<4)& 0xF0;
    quint8 Low =(b>>4) & 0x0F;
    return High |Low;
}

bool bcd2systime(QTSYSTEMTIME *pTime, const char *bcd, int nBcdLen)
{
    memset(pTime, 0, sizeof (QTSYSTEMTIME));
    pTime->wYear=(quint16)bcd2dword(bcd, 2);
    pTime->wMonth=bcd2byte(bcd[2]);
    pTime->wDay=bcd2byte(bcd[3]);
    if (nBcdLen>=7)
    {
        pTime->wHour=bcd2byte(bcd[4]);
        pTime->wMinute=bcd2byte(bcd[5]);
        pTime->wSecond=bcd2byte(bcd[6]);
    }
    return true;
}
bool systime2bcd(char *bcd, int len, const QTSYSTEMTIME &SysTime)
{
    if ((len<2) || (len>7))
    {
        return false;
    }
    char buf[64];
    QString tmpTime = QObject::tr("%1%2%3%4%5%6").arg(SysTime.wYear, 4, 10, QLatin1Char('0'))
            .arg(SysTime.wMonth, 2, 10, QLatin1Char('0')).arg(SysTime.wDay, 2, 10, QLatin1Char('0'))
            .arg(SysTime.wHour, 2, 10, QLatin1Char('0')).arg(SysTime.wMinute, 2, 10, QLatin1Char('0'))
            .arg(SysTime.wSecond, 2, 10, QLatin1Char('0'));
    memcpy(buf, tmpTime.toLocal8Bit().data(), tmpTime.length());
    Hex2Raw((quint8 *)bcd, buf, min(len*2, 14));
    return true;
}

void QDateTime2SysTime(QDateTime qTime,QTSYSTEMTIME *sysTime)
{
    sysTime->wYear = qTime.date().year();
    sysTime->wMonth = qTime.date().month();
    sysTime->wDay = qTime.date().day();
    sysTime->wHour = qTime.time().hour();
    sysTime->wMinute = qTime.time().minute();
    sysTime->wSecond = qTime.time().second();
    sysTime->wMilliseconds = qTime.time().msec();
}

//将SYSTEMTIME转换为DWORD，2000/1/1 0:0:0转换为0
quint32 SystemTime2CardTime(const QDateTime &systime)
{
    //此处使用UTC时间
    QDateTime utcTime(systime.date(), systime.time(), Qt::UTC);
    return utcTime.toTime_t();
}
//将DWORD转换为SYSTEMTIME
bool CardTime2SystemTime(quint32 dwRawTime, QDateTime& lpSystemTime)
{
    lpSystemTime = QDateTime::fromTime_t(dwRawTime).toUTC();
    ////qDebug()<<lpSystemTime.toString("yyyyMMddhhmmss");
    return lpSystemTime.isValid();
}


quint16 CRC_16(const quint8 *blk_adr, int blk_len, quint32 crc_init)
{
    quint32 crc = crc_init;
    while (blk_len--)
        crc = (quint32)(crctable[((crc>>8) ^ *blk_adr++) & 0xFF] ^ (crc << 8));
    return (quint32)(crc^0xd8);
}
quint32 CalcCardCRC16(const quint8 *blk_adr, int blk_len)
{
    return CRC_16(blk_adr, blk_len, 0x0678);
}
quint32 QDateTimeToUnixTime2000(const QDateTime &T)
{
    QDateTime BeginTime(QDate(2000,1,1),QTime(0,0));
    return (quint32)T.secsTo(BeginTime);
}

quint32 CTimeToUnixTime1970(const QDateTime &T)
{
    QDateTime BeginTime(QDate(1970,1,1),QTime(0,0));
    return (quint32)T.secsTo(BeginTime);
}
bool UnixTimeToCTime2000(quint32 UnixTime,QDateTime &T)
{
    try
    {
        QDateTime BeginTime(QDate(2000,1,1),QTime(0,0));
        T = BeginTime.addSecs(UnixTime);
        return true;
    }
    catch (...)
    {
        return false;
    }
}
quint8 BYTEToBCD(quint8 b)
{
    quint8 tmp = b % 100;
    quint8 Bcd = (((tmp/10) <<4 ) & 0xF0) | ((tmp%10) &0x0F) ;
    return Bcd;
}

quint32 Swapquint32(quint32 b)
{
    quint32 tmp= b &0xFF;
    for(int i=0;i<3;++i)
    {
        b = b >> 8;
        tmp = (tmp <<8) | (b &0xFF);
    }
    return tmp;
}

quint16 Swapquint16( quint16 b )
{
    quint16 tmp = (b <<8) & 0xFF00;
    return tmp | ((b>>8)&0x00FF);
}

quint8 Bcd2Byte( quint8 bcd )
{
    quint8 Hi= (bcd >> 4) &0x0F;
    quint8 Lo = (bcd) &0x0F;
    if(0x0F == Lo)
        return Hi;
    else
        return Hi*10+ Lo;
}

quint32 Bcd2quint32( quint32 b )
{
    return Bcd2Byte((b >>8) & 0xFF) +(b &0xFf)*100;
}

quint64 Bcd2Int( const quint8 *b,int nLen )
{
    quint64 tmp=0;
    for(int i=0;i<nLen;++i)
    {
        if(0xFF == b[i])
        {
            break;
        }
        if(0x0F == (b[i] & 0x0F))
        {
            tmp = tmp*10+Bcd2Byte(b[i]);
        }
        tmp = tmp *100 +Bcd2Byte(b[i]);
    }
    return tmp;
}

quint64 SwapBigInt( const quint8* b,int nLen )
{
    quint8 Buff[8];
    memset(Buff,0,sizeof(Buff));
    int tmpLen = nLen>8? 8:nLen;

    memcpy(Buff+8-tmpLen,b,tmpLen);

    quint8 tmpBuff[8];
    memset(tmpBuff,0,sizeof(tmpBuff));

    quint64 tmp =0;

    for(int i= 8;i>0;--i)
    {
        tmpBuff[8-i]=Buff[i-1];
    }
    memcpy(&tmp,tmpBuff,8);
    return tmp;
}

void GetPlateColor(int iIndex,QString &strColor)
{
    switch (iIndex)
    {
    case 0:
        strColor ="蓝";
        break;
    case 1:
        strColor ="黄";
        break;
    case 2:
        strColor ="黑";
        break;
    case 3:
        strColor ="白";
        break;
    default:
        strColor ="";
    }
}

void GetStrOfTerminalByF(char *Str,int nLen)
{
    for(int i=0;i < nLen;++i)
    {
        if('F' == *(Str+i) || 'f'==*(Str+i))
        {
            *(Str+i) =0;
            break;
        }
    }
    return;
}
void SysTime2QDateTime(const QTSYSTEMTIME * sysTime, QDateTime & qTime)
{
    QDate temp;
    QTime temp1;
    temp.setDate(sysTime->wYear, sysTime->wMonth, sysTime->wDay);
    qTime.setDate(temp);
    temp1.setHMS(sysTime->wHour, sysTime->wMinute, sysTime->wSecond);
    qTime.setTime(temp1);
}
quint32 SystemTime2UnixTime(const QDateTime &systime)
{
    return systime.toTime_t();
}
quint8 GetXOR(const quint8 *pBuf, int nBufLen)
{
    if(!pBuf)
        return 0;
    quint8 tmp=0;
    for(int i=0;i<nBufLen;++i)
    {
        tmp = tmp ^(quint8)pBuf[i];
    }
    return tmp;
}
QString GB2312toUtf8(const char * str, int size)
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

QByteArray Utf8toGB2312(const char* str, int size)
{
    //UTF-8 Char* to GB2312 QByteArray
    return QTextCodec::codecForName("GB18030")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str, size));
}
QByteArray Utf8toGB2312(const QString& str)
{
    return QTextCodec::codecForName("GB18030")->fromUnicode(str);
}

QString GetMD5ForBuffer(const char *szBuf, int nBufLen)
{
    QByteArray content(szBuf, nBufLen);
    QByteArray result = QCryptographicHash::hash(content, QCryptographicHash::Md5);

    QString strResult;
    strResult.append(result.toHex());
    return strResult;
}
quint16 Word2Bcd (quint16 w)
{
    return (Byte2Bcd(w % 100) << 8) || Byte2Bcd(w / 100);
}
quint8 Byte2Bcd(quint8 b)
{
    return  ((b /10) << 4) |
            (b % 10);
}
quint32  DateTimeToUnixTime2000(QDateTime ATime)
{
    QDateTime time2000(QDate(2000, 1, 1),
                       QTime(0, 0, 0));
    return ATime.toTime_t()  - time2000.toTime_t();
}
quint32  Dword2Bcd(quint32 dw)
{
    return (Word2Bcd(dw %10000) << 16) ||
            Word2Bcd(dw /10000);
}

quint16 Bcd2Word(quint16 bcd)
{
    return  Bcd2Byte((bcd >> 8) &&0xFF) + Bcd2Byte(bcd &0xFF) * 100;
}
QDateTime UnixTimeToDateTime2000(quint32 ATime)
{
    QDateTime tempDateTime(QDate(2000,1,1), QTime(0,0,0));
    int totalSec = tempDateTime.toTime_t() + ATime;
    return QDateTime::fromTime_t(totalSec);


}


QString  BCDToString(const quint8* bcd, int nSize)//传入指针有变动
{

    QString s;
    bool bZero = true;
    for(int i = 0; i < nSize; i++)
    {
        if(bZero)
        {
            if(bcd[i] != 0)
            {
                bZero = false;
                s = s + QObject::tr("%1").arg(bcd[i], 0, 16);
            }
            continue;
        }
        s = s + QObject::tr("%1").arg(bcd[1], 2, 16, QLatin1Char('0'));
    }
    return s;
}


quint32 swapDword(quint32 dw)
{
    int  i;
    quint32 A= dw & 0xFF;
    for( i= 0; i<2;i++)
    {
        dw = dw >> 8;
        return (A << 8) |(dw & 0xFF);
    }
    return A;
}
quint16 swapWORD(quint16 w)
{
    return ((w << 8) & 0xFF00) | ((w >> 8) & 0x00FF);
}
QDateTime UnixTimeToDateTime1970(quint32 ATime)
{
    return QDateTime::fromTime_t(ATime);
}
quint64 ArCharToLong(const char*Abuff,int nBuffLen)
{
    QString tempStr = QString::fromLocal8Bit(Abuff, nBuffLen);
    return tempStr.toULongLong();
}
quint32 DateTimeToUnixTime1970(QDateTime ATime)
{QDateTime time1970(QDate(1970,0,0),QTime(0,0,0));
    return ATime.toTime_t()-time1970.toTime_t();


}

quint8 qLoByte(quint16 wValue)
{
    return (quint8)(wValue & 0xFF);
}

quint8 qHiByte(quint16 wValue)
{
    return (quint8)((wValue >> 8) & 0xFF );
}

int IntToArChar(void * szBuf, int nBufLen, quint32 ul)
{
    quint8* pB = NULL;
    if(nBufLen < 1)
    {
        return -1;
    }
    for(int i = nBufLen - 1; i >= 0; i--)
    {
        pB = (quint8*)szBuf;
        pB += i;
        *pB = ul % 10 + '0';
        ul = ul / 10;
    }
    return ul;
}

bool setSystemDateTime(const QDateTime &sDate)
{
#ifndef Q_OS_WIN
    QProcess::execute("date -s \"" + sDate.toString("yyyy-MM-dd hh:mm:ss") + "\"");
    QProcess::execute("hwclock -w");
#else
    SYSTEMTIME st;
    GetSystemTime(&st);
    st.wYear = sDate.date().year();
    st.wMonth = sDate.date().month();
    st.wDay = sDate.date().day();
    st.wHour = sDate.time().hour();
    st.wMinute = sDate.time().minute();
    st.wSecond = sDate.time().second();
    SetSystemTime(&st);
    SetLocalTime(&st);
#endif
    return true;
}

quint64 getDiskSpaceFree(const QString &sDir)
{
#ifdef Q_OS_UNIX
    struct statfs diskInfo;
    int ret = statfs(sDir.toLocal8Bit().data(),&diskInfo);
    quint64 totalSize = 0;
    ////qDebug() << "get space ret is " << ret << diskInfo.f_bsize << diskInfo.f_bfree;
    if(ret == 0)
    {
        quint64 totalBlocks = diskInfo.f_bsize;
        totalSize = totalBlocks * diskInfo.f_bfree;
    }
    return totalSize;
#else
    return 0;
#endif
}
bool HasEntryFlag(quint8 bPassFlag)
{
    LaneType temp=Byte2LaneType(bPassFlag);
    if(temp==LaneType_MtcIn || temp==LaneType_EtcIn)
        return true;
    else
        return false;
}

bool isDiskSpaceEnough()
{
    QSettings settings("./Lane.ini", QSettings::IniFormat);
    int mainDisk = settings.value("DiskCheck/SDCheck", 0).toInt();
    int imageDisk = settings.value("DiskCheck/ImageCheck", 0).toInt();
    int sdDisk = settings.value("DiskCheck/SDCheck", 0).toInt();
    if(mainDisk > (getDiskSpaceFree("/") >> 20))
    {
        return false;
    }
    if(imageDisk > (getDiskSpaceFree("/Program_Data") >> 20))
    {
        return false;
    }
    if(sdDisk > (getDiskSpaceFree("/SDHC") >> 20))
    {
        return false;
    }
    return true;
}

void getMemoryInfo(quint32 *totalMemory, quint32 *freeMemory)
{
    *totalMemory = 0;
    *freeMemory = 0;
#ifndef Q_OS_WIN
    struct sysinfo sinfo;
    sysinfo(&sinfo);
    *totalMemory = sinfo.totalram;
    *freeMemory = sinfo.freeram;
#endif
}
