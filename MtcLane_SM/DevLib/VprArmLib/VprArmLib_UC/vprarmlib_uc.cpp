#include "vprarmlib_uc.h"
#include <QLibrary>
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
//using namespace mini;
static QObject *m_hParentVPRHandle=NULL;		//	窗口句柄
static quint32 m_nDataMsgID;			//	车牌识别消息标识
static QLibrary library;
static bool m_bLoaded = false;
typedef int (*Func_VPR_RegisterCallback)(Func_VPR_NotifyFunc NotifyFunc) ;
typedef int (*Func_VPR_Connect)(const int *lpszIP, unsigned int nPort, unsigned int nComPort);
typedef int (*Func_VPR_Disconnect)();
typedef int (*Func_VPR_Capture)();
typedef int (*Func_VPR_GetPlate)(int *nColor,char* lpszPlate,unsigned int *nRating);
typedef int (*Func_VPR_GetBigImg)(unsigned char* lpImgBuf,unsigned int *nSize);
typedef int (*Func_VPR_GetSmallImg)(unsigned char* lpImgBuf,unsigned int *nSize);
typedef int (*Func_VPR_GetBinImg)(unsigned char* lpImgBuf,unsigned int *nSize,unsigned int *nWidth,unsigned int* nHeight);
typedef int (*Func_VPR_GetVer)(char* lpszVer,char* lpszVender);
typedef int (*Func_VPR_CheckStatus)(unsigned char* nStatus);

Func_VPR_RegisterCallback funcReg = NULL;
Func_VPR_Capture funcCapture = NULL;
Func_VPR_CheckStatus funcCheckStatus = NULL;
Func_VPR_Connect funcConnect = NULL;
Func_VPR_Disconnect funcDisconnect = NULL;
Func_VPR_GetBigImg funcGetBigImg = NULL;
Func_VPR_GetBinImg funcGetBinImg = NULL;
Func_VPR_GetPlate funcGetPlate = NULL;
Func_VPR_GetSmallImg funcGetSmallImg = NULL;
Func_VPR_GetVer funcGetVer = NULL;

static void callbakFunc(quint32 index)
{
    qDebug() << "vpr call bak func exec";
    QCoreApplication::postEvent((QObject*)m_hParentVPRHandle,
                                new VPREVENT(m_nDataMsgID, index));

}

bool loadLibrary()
{
    if(m_bLoaded)
    {
        return true;
    }
    if(!QFile::exists("/lib/VprCtrl.so"))
    {
        qDebug("lib vpr ctrl is not exists...");
        return false;
    }
    library.setFileName("/lib/VprCtrl.so");
    funcReg = (Func_VPR_RegisterCallback)library.resolve("VPR_RegisterCallback");
    funcCapture = (Func_VPR_Capture)library.resolve("VPR_Capture");
    funcDisconnect = (Func_VPR_Disconnect)library.resolve("VPR_Disconnect");
    funcGetBigImg = (Func_VPR_GetBigImg)library.resolve("VPR_GetBigImg");
    funcGetBinImg = (Func_VPR_GetBinImg)library.resolve("VPR_GetBinImg");
    funcGetPlate = (Func_VPR_GetPlate)library.resolve("VPR_GetPlate");
    funcGetSmallImg = (Func_VPR_GetSmallImg)library.resolve("VPR_GetSmallImg");
    funcGetVer = (Func_VPR_GetVer)library.resolve("VPR_GetVer");
    funcConnect = (Func_VPR_Connect)library.resolve("VPR_Connect");
    funcCheckStatus = (Func_VPR_CheckStatus)library.resolve("VPR_CheckStatus");
    if(funcReg == NULL ||
            funcCapture == NULL ||
            funcCheckStatus == NULL ||
            funcConnect == NULL ||
            funcDisconnect == NULL ||
            funcGetBigImg == NULL ||
            funcGetBinImg == NULL ||
            funcGetPlate == NULL ||
            funcGetSmallImg == NULL ||
            funcGetVer == NULL)
    {
        qDebug("load library file fail...");
        return false;
    }
    qDebug("load VprCtrl.so file success");
    m_bLoaded = true;
    return true;
}

int VPR_RegisterMessage(QObject* hHandle, quint32 nDataMsgID)
{
    //logmsg(QObject::tr("register message begin, data msg id is [%1]").arg(nDataMsgID));
    if(!m_bLoaded)
    {
        loadLibrary();
    }
    if(!m_bLoaded)
    {
        return false;
    }
    int ret = -1;
    if(hHandle != NULL)
    {
        m_hParentVPRHandle = hHandle;
        m_nDataMsgID = nDataMsgID;
        if(funcReg != NULL)
        {
            qDebug() << "begin reg vpr arm uc";
            ret = funcReg(callbakFunc);
        }
        else
        {
            //logmsg(QObject::tr("register message func is null, lib is not load"));
        }
    }
    return ret;
}
int VPR_RegisterCallback(Func_VPR_NotifyFunc NotifyFunc)
{
    return VPR_ERR_NO_SUPPORT;
}
int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2)
{
    int ret = -1;
    if(!m_bLoaded)
    {
        loadLibrary();
    }
    if(m_bLoaded)
    {
        ret = funcConnect((const int*)lpszConnPara1, QString(lpszConnPara2).toUInt(), 0);
        //logmsg(QObject::tr("vpr connect ip is [%1], port [%2], connect result[%3]")
               //.arg(lpszConnPara1).arg(lpszConnPara2).arg(ret));
        qDebug() << lpszConnPara1 << lpszConnPara2 << ret;
    }
    return ret;
}
int VPR_Disconnect(void)
{
    int ret = -1;
    if(funcDisconnect != NULL)
    {
        ret = funcDisconnect();
        //logmsg(QObject::tr("vpr disconnect execute, result is [%1]").arg(ret));
    }
    return ret;
}
int VPR_Capture(void)
{
    int ret = -1;
    if(funcCapture != NULL)
    {
        ret = funcCapture();
        //logmsg(QObject::tr("vpr capture execute, result is [%1]").arg(ret));
    }
    return ret;
}
int VPR_GetPlate(quint32 nIndex, int &nColor,  char* lpszPlate, quint32 &nRating)
{
    int ret = -1;
    if(funcGetPlate != NULL)
    {
        ret = funcGetPlate(&nColor, lpszPlate, &nRating);
        //logmsg(QObject::tr("vpr get plate execute, index[%1], color[%2], plate[%3], rating[%4]")
               //.arg(nIndex).arg(nColor).arg(lpszPlate).arg(nRating));
    }
    return ret;
}
int VPR_GetBigImg(quint32 nIndex, quint8 * lpImgBuf, quint32 &nSize)
{
    int ret = -1;
    if(funcGetBigImg != NULL)
    {
        ret = funcGetBigImg(lpImgBuf, &nSize);
        //logmsg(QObject::tr("vpr get big img execute, index[%1], size[%2]").arg(nIndex).arg(nSize));
    }
    return ret;
}
int VPR_GetSmallImg(quint32 nIndex, quint8 * lpImgBuf, quint32 &nSize)
{
    int ret = -1;
    if(funcGetSmallImg != NULL)
    {
        ret = funcGetSmallImg(lpImgBuf, &nSize);
        //logmsg(QObject::tr("vpr get small img execute, index[%1], size[%2]").arg(nIndex).arg(nSize));
    }
    return ret;
}
int VPR_GetBinImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize, quint32 &nWidth, quint32 &nHeight)
{
    int ret = -1;
    if(funcGetBinImg != NULL)
    {
        ret = funcGetBinImg(lpImgBuf, &nSize, &nWidth, &nHeight);
        //logmsg(QObject::tr("vpr get bin img execute, index[%1], size[%2]").arg(nIndex).arg(nSize));
    }
    return ret;
}
int VPR_GetAllVehInfo(quint32 nIndex, CAutoDetectedVehInfo *pVehInfo)
{
    int ret = -1;
    int tmpRet1, tmpRet2, tmpRet3, tmpRet4;
    //logmsg("vpr get all veh info execute...");
    if(funcGetPlate != NULL && funcGetBigImg != NULL && funcGetBinImg != NULL &&
            funcGetSmallImg != NULL)
    {
        tmpRet1 = funcGetPlate(&pVehInfo->nColor, pVehInfo->lpszPlate, &pVehInfo->nRating);
        tmpRet2 = funcGetBigImg(pVehInfo->lpBigImgBuf, &pVehInfo->nBigImgSize);
        tmpRet3 = funcGetBinImg(pVehInfo->lpBinImgBuf, &pVehInfo->nBinImgSize,
                      &pVehInfo->nBinImgWidth, &pVehInfo->nBinImgHeight);
        tmpRet4 = funcGetSmallImg(pVehInfo->lpSmallImgBuf, &pVehInfo->nSmallImgSize);
        if(tmpRet1 == -1)
        {
            ret = -1;
        }
        if(tmpRet2 == -1)
        {
            ret = -1;
        }
        if(tmpRet3 == -1)
        {
            ret = -1;
        }
        if(tmpRet4 == -1)
        {
            ret = -1;
        }
    }
    return ret;
}

int VPR_GetVer(char* lpszVer, char* lpszVender, char* lpszDevType)
{
    int ret = -1;
    if(funcGetVer != NULL)
    {
        ret = funcGetVer(lpszVer, lpszVender);
        //logmsg(QObject::tr("vpr get ver  execute, Ver[%1], Vender[%2], DevType[%3]")
               //.arg(lpszVer).arg(lpszVender).arg(lpszDevType));
    }
    return ret;
}
