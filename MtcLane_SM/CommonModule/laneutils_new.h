#ifndef LANEUTILS_NEW_H
#define LANEUTILS_NEW_H

#include <QtGlobal>
#include <cstdarg>
#include <cstdio>
#include <QString>
#include <cstdlib>
#include <QDateTime>
#include <QHostAddress>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDateTime>
#include <QTextCodec>
#include "LaneType.h"
#define HIGH8BIT(w) ((quint8)((w >> 8) & 0xff))
#define LOW8BIT(w) ((quint8)(w & 0xff))
using namespace std;

int ExtractComId(const char *szConnStr);
bool TransToStdComPortName(char *szDest, size_t nMaxLen, const char *szSource);
void SetBit(quint32 *pdwBase, int nIndex, bool bStatus);
bool GetBitStatus(const quint32 dwBase, int nIndex);
//将整数转化为指定长度的十进制字符数组，前面补'0' √
int ulong2StrBuf(char *szBuf, size_t nBufLen, unsigned long ul);
//将__int64转化为指定长度的十进制字符串，前面补'0' √
quint64 ulonglong2DecStr(char *szBuf, size_t nBufLen, quint64 ull);
//将有可能不以0字符结尾的十进制字符串转换成整数 √
unsigned long StrBuf2Ulong(const char *szBuf, size_t nBufLen);
//获取双字节字符的个数 √
int GetDbcsCharNum(const char *szBuf);
//计算字符数组的异或值 √
quint8 GetXOR(const quint8 *pBuf, int nBufLen);

//将SYSTEMTIME转换为格式为"YYYY/MM/DD HH:NN:SS"的字符串指针（19字节，0字符结尾），指针指向的地址为函数内的静态地址 √
char *FormatTime(const QDateTime &SysTime);
//将SYSTEMTIME转换为格式为"YYYY/MM/DD HH:NN:SS"的字符串指针（19字节，函数实现未实现szTime为0字符结尾） √
void FormatTime(const QDateTime &SysTime, char *szTime);
//将SYSTEMTIME转换为格式为"YYYYMMDDHHNNSS"的字符串指针（14字节，0字符结尾），指针指向的地址为函数内的静态地址 √
char *FormatTimeToChar14(const QDateTime &SysTime);
//将SYSTEMTIME转换为格式为"YYYYMMDDHHNNSS"的字符串指针（参数指针需预分配14字节以上空间） √
void FormatTimeToChar14(const QDateTime &SysTime,char *sTime14);
void DateTimeToChar14(const QDateTime &dateTime, char *sTime14);
void DateTimeToChar14(const QTSYSTEMTIME& sysTime, char *sTime14);
//将char14数组里面的时间转化为SYSTEMTIME，szTime需要大于或等于14字节  ×××××××××
bool ConvertChar14ToSystemTime(QTSYSTEMTIME *pSysTime, const char *szTime);
bool ConvertChar14ToSystemTime(QDateTime *pSysTime, const char *szTime);
//将BCD码的时间转换为SYSTEMTIME，BCD码可以为4字节（只包含年月日）或7字节（除年月日外，还有时分秒）√
bool bcd2systime(QDateTime *pTime, const char *bcd, int nBcdLen=4);
//将SYSTEMTIME转换为BCD码的时间，len应在2-7之间，即可以只转换年（2字节），或转换年月日时分秒（7字节）
bool systime2bcd(char *bcd, int len, const QDateTime &SysTime);

bool bcd2systime(QTSYSTEMTIME *pTime, const char *bcd, int nBcdLen=4);
bool systime2bcd(char *bcd, int len, const QTSYSTEMTIME &SysTime);

bool Hex2Raw(quint8 *pData, const char *szMsg, int nMsgLen);
quint32 bcd2dword(const char *bcd, int len);
quint64 bcd2dword64(const char *bcd, int len);
bool ll2bcd(char *bcd, int bcd_len, quint64 d64);
char *HexStr(const quint8 *pBuf, int nBufLen);
void Raw2HexStr(char *pStr, const quint8 *pRaw, int nRawLen);

int ArCharToInt(const char *szBuff, int nBuffLen);
bool IsValidIP(const char *szIP);
bool DirectoryExists(const char *szPathName);
bool FileExists(const char *szFileName);
//实现原理为，首先判断目标目录是否存在重名文件，有则删除，后将源文件复制到目标目录，同时删除源文件~
bool MoveFileReplaceExisting(QString lpExistingFileName, QString lpNewFileName);
//// 移动一个文件夹下的文件到另一文件夹
bool MoveFilesInDirectory(QString lpSourceDirectory, QString lpTargetDirectory, bool bIncludeChildrenFolder=false);
bool WriteBufToFile(QString lpImageName, const char * lpBuffer, quint32 nBufferLen);
bool ReadBufFromFile(QByteArray & lpBuffer, quint32 &nBufferLen, QString & lpImageName);
////创建多级目录。传入目录必须带"\\"
bool ForceDirectories(const char *szPathName);

//CString GetDiskInfo();
inline bool IsWindowsNT()
{
    #ifdef Q_OS_WIN32
        return true;
    #else
        return false;
    #endif
}
//高低位转换
quint8 Swap4Bits(quint8 b );

inline quint8 bcd2byte(quint8 bcd)
{
        quint8 lo=bcd&0x0F;
        quint8 hi=(bcd>>4)&0x0F;
        if (0xF==lo)
        {
                return hi;
        }
        else
        {
                return hi*10+lo;
        }
}
quint32 SystemTime2UnixTime(const QDateTime &systime);
void QDateTime2SysTime(QDateTime qTime, QTSYSTEMTIME * sysTime);
void SysTime2QDateTime(const QTSYSTEMTIME * sysTime, QDateTime & qTime);
//将SYSTEMTIME转换为DWORD，2000/1/1 0:0:0转换为0
quint32 SystemTime2CardTime(const QDateTime &systime);
//将DWORD转换为SYSTEMTIME
bool CardTime2SystemTime(quint32 dwRawTime, QDateTime &lpSystemTime);
quint16 CRC_16(const quint8 *blk_adr, int blk_len, quint32 crc_init);
quint32 CalcCardCRC16(const quint8 *blk_adr, int blk_len);
quint32 QDateTimeToUnixTime2000(const QDateTime &T);
quint32 CTimeToUnixTime1970(const QDateTime &T);
bool UnixTimeToCTime2000(quint32 UnixTime,QDateTime &T);
quint8 BYTEToBCD(quint8 b);
quint32 Swapquint32(quint32 b);
quint16 Swapquint16( quint16 b );
quint8 Bcd2Byte( quint8 bcd );
quint32 Bcd2quint32( quint32 b );
quint64 Bcd2Int( const quint8 *b,int nLen );
quint64 SwapBigInt( const quint8* b,int nLen );
void GetPlateColor(int iIndex,QString &strColor);
void GetStrOfTerminalByF(char *Str,int nLen);
//gb2312 to UTF8
QString GB2312toUtf8(const char * str, int size = -1);
//UTF8 to gb2312
QByteArray Utf8toGB2312(const char* str, int size = -1);
QByteArray Utf8toGB2312(const QString& str);
//获取MD5
QString GetMD5ForBuffer(const char *szBuf, int nBufLen);
quint16 Word2Bcd(quint16);
quint8 Byte2Bcd(quint8 b);
quint32  DateTimeToUnixTime2000(QDateTime ATime);
quint32  Dword2Bcd(quint32 dw);
quint16 Bcd2Word(quint16 bcd);
QDateTime UnixTimeToDateTime2000(quint32 ATime);
QString  BCDToString(const quint8* bcd, int nSize);
quint32 swapDword(quint32 dw);
quint16 swapWORD(quint16 w);
quint64 ArCharToLong(const char*Abuff,int nBuffLen);
QDateTime UnixTimeToDateTime1970(quint32 ATime);
quint32 DateTimeToUnixTime1970(QDateTime ATime);
quint8 qLoByte(quint16 wValue);
quint8 qHiByte(quint16 wValue);
int IntToArChar(void * szBuf, int nBufLen, quint32 ul);
//设置系统时间
bool setSystemDateTime(const QDateTime &sDate);
//获取图片路径
inline QString getCapPicPath()
{
    return "./capture/";
}

//获取空间字节数
quint64 getDiskSpaceFree(const QString& sDir);
//获取内存情况
void getMemoryInfo(quint32 *totalMemory, quint32 *freeMemory);
//判断是否空间允许做业务
bool isDiskSpaceEnough();
//ETC出口用-----------Add BY xujingliang
bool HasEntryFlag(quint8 bPassFlag);
#endif // LANEUTILS_NEW_H
