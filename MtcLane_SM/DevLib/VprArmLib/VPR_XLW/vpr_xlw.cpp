#include "vpr_xlw.h"
#include <QString>
#include <QDebug>
#include <unistd.h>
#include <QCoreApplication>
#include <QTextCodec>
class XLW_VehicleInfo
{
public:
    quint32 m_nIndex;			//	索引
    quint32 m_dwCarID;                  //	车辆ID
    char m_szPlate[80];                 //	车牌号（含车牌颜色）
    quint8 m_baBigImg[100*1024];	//	车辆大图
    int m_nBigImgSize;			//	车辆大图实际大小
    quint8 m_baSmallImg[10*1024];	//	车辆小图（牌照区域）
    int m_nSmallImgSize;		//	车辆小图实际大小
public:
    XLW_VehicleInfo()
    {
        m_nIndex=0;
        m_dwCarID=0;
        m_szPlate[0]=0;
        m_nBigImgSize=0;
        m_nSmallImgSize=0;
    }
    ~XLW_VehicleInfo()
    {
    }
public:
    // 初始化，清空数据
    void Init(int nIndex, quint32 dwCarID)
    {
        m_nIndex = nIndex;
        m_dwCarID = dwCarID;
        m_szPlate[0] = 0;
        m_nBigImgSize = 0;
        m_nSmallImgSize = 0;
    }
    // 保存车牌
    bool SavePlate(const char *szPlate, quint32 dwCarID)
    {
        if (NULL == szPlate)
        {
            return false;
        }

//        _snprintf_s(m_szPlate, sizeof(m_szPlate), _TRUNCATE, "%s", szPlate);
        strncpy(m_szPlate,szPlate,sizeof(m_szPlate));
        return true;
    }
    // 保存大图
    bool SaveBigImg(quint32 dwCarID, quint8* pbImage, quint32 dwSize)
    {
        m_nBigImgSize=0;
        if (0==dwSize || dwSize>sizeof(m_baBigImg))
        {
            return false;
        }
        memcpy(m_baBigImg, pbImage, dwSize);
        m_nBigImgSize=dwSize;
        return true;
    }
    // 保存小图
    bool SaveSmallImg(quint32 dwCarID, quint16 wImageWidth, quint16 wImageHigh,
                      quint8 bType, quint16 wSize, quint8 *pbImage)
    {
        // NOTE: 估算BMP长度为wImageWidth*wImageHigh*3+1024，保存为JPG格式不好估算，
        //	因小图较小，为了简化，采用定长数组
        HRESULT rs;
        m_nSmallImgSize=0;

        rs=Yuv2JPG(m_baSmallImg, sizeof(m_baSmallImg), &m_nSmallImgSize, pbImage, wImageWidth, wImageHigh);
        if (S_OK!=rs ||(m_nSmallImgSize>sizeof m_baSmallImg))
        {
            m_nSmallImgSize=0;
            return false;
        }
        return m_nSmallImgSize>0;
    }
};
// 车牌识别结果缓冲
XLW_VehicleInfo g_VehicleInfoArray[3];
int g_nCurrentIndex=0;

QObject *m_hParentVPRHandle=NULL;		//	窗口句柄
quint32 m_nDataMsgID;			//	车牌识别消息标识
bool m_bDevOpened=false;			//  设备是否已经初始化
// 信路威视频处理系统句柄
HV_HANDLE m_hVPRHandle=NULL;

QString GB2312toUtf8(const char * str, int size = -1)
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

int TransVehPlateColor(const QString &sColor)
{
    if (0==sColor.compare(QObject::tr("蓝")))
    {
        return VP_COLOR_BLUE;
    }
    if (0==sColor.compare(QObject::tr("黄")))
    {
        return VP_COLOR_YELLOW;
    }
    if (0==sColor.compare(QObject::tr("白")))
    {
        return VP_COLOR_WHITE;
    }
    if (0==sColor.compare(QObject::tr("黑")))
    {
        return VP_COLOR_BLACK;
    }
    return VP_COLOR_NONE;
}
//开始传送结果回调(用以确保数据传送的完整性)
int onStartRecvResult(void *pFirstParameter, quint32 dwCarID)
{
    //	LogFmtMsg("识别开始，车辆ID=%u", dwCarID);
    qDebug()<<"识别开始，车辆ID="<<dwCarID;
    // 转入下一个数据缓存，等待接收车牌数据
    // NOTE: 待接收完成车牌和图像数据后再发送消息，通知上层应用取数据

    g_nCurrentIndex = (g_nCurrentIndex+1) % ((sizeof(g_VehicleInfoArray) / sizeof(g_VehicleInfoArray)));
    g_VehicleInfoArray[g_nCurrentIndex].Init(g_nCurrentIndex, dwCarID);
    return 0;
}
//结束传送结果回调(用以确保数据传送的完整性)
int onEndRecvResult(void *pFirstParameter, quint32 dwCarID)
{
        qDebug()<<"识别结束，车辆ID="<<dwCarID;
        return 0;
}

//车牌号回调函数
int onPlateNoCallBack(void *pFirstParameter, qint32 dwCarID, char* pcPlateNo, quint64 dw64TimeMs)
{
        qDebug()<<"识别到车牌，车辆ID="<<dwCarID<<"车牌="<<pcPlateNo;
        if (g_nCurrentIndex >= 0 && g_nCurrentIndex < sizeof(g_VehicleInfoArray)/sizeof(XLW_VehicleInfo))
        {
                bool bRlt;
                bRlt=g_VehicleInfoArray[g_nCurrentIndex].SavePlate(pcPlateNo, dwCarID);

                if (bRlt)
                {
                    QCoreApplication::postEvent((QObject*)m_hParentVPRHandle, new VPREVENT(m_nDataMsgID, g_nCurrentIndex));
                    //			::PostMessage(m_hParentVPRHandle, m_nDataMsgID, g_nCurrentIndex, 0);
                }
        }
        else
        {
                qDebug()<<"onPlateNoCallBack程序逻辑错误";
        }
        return 0;
}

//车牌小图回调函数
int onSmallImgCallBack(void *pFirstParameter, qint32 dwCarID, quint16 wImageWidth, quint16 wImageHigh,
        quint8 bType, quint16 wSize, quint8 *pbImage, quint64 dwTimeMs)
{
        qDebug()<<"车牌小图，车辆ID="<<dwCarID<<"图像尺寸="<<wImageWidth<<","<<wImageHigh<<"类型="<<bType<<"大小="<<wSize;
        if (g_nCurrentIndex >= 0 && g_nCurrentIndex < sizeof(g_VehicleInfoArray)/sizeof(XLW_VehicleInfo))
        {
                g_VehicleInfoArray[g_nCurrentIndex].SaveSmallImg(dwCarID, wImageWidth, wImageHigh,
                        bType, wSize, pbImage);
        }
        else
        {
                qDebug()<<"onSmallImgCallBack程序逻辑错误";
        }
        return 0;
}

//车牌大图回调函数
int onBigImgCallBack(void *pFirstParameter, quint32 dwCarID, quint16 wImageWidth, quint16 wImageHigh,
        quint8 bType, quint16 wSize, quint8* pbImage, quint16 wImageID, quint16 wHighImgFlag, quint16 wPlateWidth, quint16 wPlateHigh, quint64 dwTimeMs)
{
//        LogFmtMsg("车牌大图，车辆ID=[%u]，图像尺寸=[%u*%u]，类型=[%d]，大小=[%u]，图像ID=[%u]", dwCarID,
//                wImageWidth, wImageHigh, bType, wSize, wImageID);
        qDebug()<<"车牌小图，车辆ID="<<dwCarID<<"图像尺寸="<<wImageWidth<<","<<wImageHigh<<"类型="<<bType<<"大小="<<wSize;
        quint32 dwSize = 0;
        if( (wHighImgFlag & 0xFF00) != 0 )
        {
                dwSize = wSize + (int)((wHighImgFlag & 0x00FF) << 16);
                qDebug()<<"高清图像，图像大小="<<dwSize;
        }
        else
        {
                dwSize = wSize;
        }
        bool bRlt;
        qDebug() << "size" << sizeof(g_VehicleInfoArray)/sizeof(XLW_VehicleInfo);
        if (g_nCurrentIndex >= 0 && g_nCurrentIndex < (sizeof(g_VehicleInfoArray)/sizeof(XLW_VehicleInfo)))
        {
                bRlt=g_VehicleInfoArray[g_nCurrentIndex].SaveBigImg(dwCarID, pbImage, dwSize);

                if (bRlt)
                {
                    QCoreApplication::postEvent((QObject*)m_hParentVPRHandle, new VPREVENT(m_nDataMsgID, g_nCurrentIndex));
//			::PostMessage(m_hParentVPRHandle, m_nDataMsgID, g_nCurrentIndex, 0);
                }
        }
        else
        {
                qDebug()<<"onSmallImgCallBack程序逻辑错误";
        }
        return 0;
}

// 将车牌识别结果按格式要求进行转换
bool ConvertGDWPlate(int &nColor, QString &sPlate, const QString &sFullPlate)
{
    int len = sFullPlate.length();
    if (len < 2)
    {
        // 全车牌中没有颜色
        nColor = VP_COLOR_NONE;
        sPlate = sFullPlate;
        return true;
    }
    QString sColor = sFullPlate.left(1);
    nColor = TransVehPlateColor(sColor);
    if (VP_COLOR_NONE == nColor)
    {
        // 如果前2个字符不是颜色，假定颜色未识别出
        sPlate = sFullPlate;
        if (0 == sPlate.compare("无车牌"))
        {
            // 高德威车牌识别设备在没有车牌号时会上送“无车牌”
            sPlate = "";
        }
    }
    else
    {
        sPlate = sFullPlate.mid(1, sFullPlate.length()-1);
    }
    return true;
}




inline bool VerifyVPRResultIndex(quint32 n)
{
    return n >= 0 && n < sizeof(g_VehicleInfoArray) / sizeof(XLW_VehicleInfo);
}

int VPR_RegisterCallback(Func_VPR_NotifyFunc NotifyFunc)
{
    // 暂不支持回调函数接口
    return VPR_ERR_NO_SUPPORT;
}


int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2)
{
    //TODO
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    // 车牌识别日志文件
    //TODO
//    CFileLogger::GetLogger()->ModifyLogFileName("VPR.log");
    // 打开设备
    char szIP[260];
//    _snprintf_s(szIP, _countof(szIP), _TRUNCATE, "%s", lpszConnPara1);
    strncpy(szIP,lpszConnPara1,sizeof(szIP));
    m_hVPRHandle=OpenHv(szIP);

    if (E_HV_INVALID_HANDLE==m_hVPRHandle)
    {
        qDebug()<<"调用OpenHv失败";
        return VPR_ERR_UNKNOWN;
    }
    else
        qDebug()<<"打开成功";
    // 注册回调函数
    SetHvCallBack(m_hVPRHandle, (void *)onStartRecvResult,	NULL, 0, _TYPE_CARINFO_BEGIN);
    SetHvCallBack(m_hVPRHandle, (void *)onEndRecvResult,	NULL, 0, _TYPE_CARINFO_END);
    SetHvCallBack(m_hVPRHandle, (void *)onPlateNoCallBack,	NULL, 0, _TYPE_PLATE_STR);
    SetHvCallBack(m_hVPRHandle, (void *)onSmallImgCallBack,	NULL, 0, _TYPE_SMALL_IMAGE);
    SetHvCallBack(m_hVPRHandle, (void *)onBigImgCallBack,	NULL, 0, _TYPE_BIG_IMAGE);
    m_bDevOpened=true;
    return VPR_OK;
}
int VPR_Disconnect(void)
{
        if (!m_bDevOpened)
        {
                return VPR_OK;
        }
        if (E_HV_INVALID_HANDLE == m_hVPRHandle)
        {
                m_bDevOpened=false;
                return VPR_OK;
        }
        // 关闭数据接收
        SetHvCallBack(m_hVPRHandle, NULL, NULL, 0, _TYPE_CARINFO_BEGIN);
        SetHvCallBack(m_hVPRHandle, NULL, NULL, 0, _TYPE_CARINFO_END);
        SetHvCallBack(m_hVPRHandle, NULL, NULL, 0, _TYPE_PLATE_STR);
        SetHvCallBack(m_hVPRHandle, NULL, NULL, 0, _TYPE_SMALL_IMAGE);
        SetHvCallBack(m_hVPRHandle, NULL, NULL, 0, _TYPE_BIG_IMAGE);

        HRESULT hRlt;
        hRlt=CloseHv(m_hVPRHandle);
        if (S_OK!=hRlt)
        {
                qDebug()<<"CloseHv函数返回失败，错误代码="<<hRlt;
                return VPR_ERR_UNKNOWN;
        }
        m_bDevOpened=false;
        return VPR_OK;
}

int VPR_Capture(void)
{
        if (!m_bDevOpened)
        {
                return VPR_ERR_UNKNOWN;
        }
        HRESULT rs=HV_ForceSend(m_hVPRHandle);
        if (S_OK!=rs)
        {
                qDebug()<<"调用HV_ForceSend函数失败，错误代码= "<< rs;
                return VPR_ERR_UNKNOWN;
        }
        return VPR_OK;
}
int VPR_GetPlate(quint32 nIndex, int &nColor, char* lpszPlate, quint32 &nRating)
{
    //TODO
//       AFX_MANAGE_STATE(AfxGetStaticModuleState());
       if (!VerifyVPRResultIndex(nIndex))
       {
               return VPR_ERR_PARAM;
       }
       QString sPlate;
       if (!ConvertGDWPlate(nColor, sPlate, GB2312toUtf8(g_VehicleInfoArray[nIndex].m_szPlate)))
       {
               // 转换车牌错误，假定车牌识别失败
               nColor=VP_COLOR_NONE;
               if (lpszPlate)
               {
                       lpszPlate[0]=0;
               }
               return VPR_ERR_UNKNOWN;
       }
       else
       {
               if (lpszPlate)
               {
//                       _snprintf_s(lpszPlate, 20, _TRUNCATE, "%s", sPlate.toLocal8Bit().data());
                       strncpy(lpszPlate,sPlate.toLocal8Bit().data(),20);
               }
               else
               {
                       return VPR_ERR_RANGE;
               }
       }
       // 设备不返回车牌可信度，假定可信度为99
       nRating=99;
       return VPR_OK;
}

int VPR_GetBigImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize)
{
    //TODO
       //AFX_MANAGE_STATE(AfxGetStaticModuleState());
    qDebug() << "lib get big image";
       if (!VerifyVPRResultIndex(nIndex))
       {
               // 参数错误
               return VPR_ERR_PARAM;
       }
       qDebug() << "lib get big image" << nIndex << g_VehicleInfoArray[nIndex].m_nBigImgSize << nSize;

       if (0>=g_VehicleInfoArray[nIndex].m_nBigImgSize)
       {
               // 无图像
               nSize=0;
               return VPR_ERR_NO_DATA;
       }
       if (NULL==lpImgBuf || nSize<(UINT)g_VehicleInfoArray[nIndex].m_nBigImgSize)
       {
               // 图像大小大于缓冲区
               nSize=g_VehicleInfoArray[nIndex].m_nBigImgSize;
               return VPR_ERR_RANGE;
       }
       nSize=g_VehicleInfoArray[nIndex].m_nBigImgSize;
       memcpy(lpImgBuf, g_VehicleInfoArray[nIndex].m_baBigImg, g_VehicleInfoArray[nIndex].m_nBigImgSize);
       return VPR_OK;
}

int  VPR_GetSmallImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize)
{
    //TODO
       //AFX_MANAGE_STATE(AfxGetStaticModuleState());
       if (!VerifyVPRResultIndex(nIndex))
       {
               // 参数错误
               return VPR_ERR_PARAM;
       }
       if (0>=g_VehicleInfoArray[nIndex].m_nSmallImgSize)
       {
               // 无图像
               nSize=0;
               return VPR_ERR_NO_DATA;
       }
       if (NULL==lpImgBuf || nSize<(UINT)g_VehicleInfoArray[nIndex].m_nSmallImgSize)
       {
               // 图像大小大于缓冲区
               nSize=g_VehicleInfoArray[nIndex].m_nSmallImgSize;
               return VPR_ERR_RANGE;
       }
       nSize=g_VehicleInfoArray[nIndex].m_nSmallImgSize;
       memcpy(lpImgBuf, g_VehicleInfoArray[nIndex].m_baSmallImg, g_VehicleInfoArray[nIndex].m_nSmallImgSize);
       return VPR_OK;
}

int VPR_GetBinImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize, quint32 &nWidth, quint32 &nHeight)
{
       // 暂不支持取二值化图像
       return VPR_ERR_NO_SUPPORT;
}

 int VPR_GetAllVehInfo(quint32 nIndex, CAutoDetectedVehInfo *pVehInfo)
{
     //TODO
//       AFX_MANAGE_STATE(AfxGetStaticModuleState());
       if (!VerifyVPRResultIndex(nIndex))
       {
               // 参数错误
               return VPR_ERR_PARAM;
       }
       //取大图
       if (VPR_OK!=VPR_GetBigImg(nIndex, pVehInfo->lpBigImgBuf, pVehInfo->nBigImgSize))
       {
               pVehInfo->nBigImgSize=0;
       }
       //取小图
       if (VPR_OK!=VPR_GetSmallImg(nIndex, pVehInfo->lpSmallImgBuf, pVehInfo->nSmallImgSize))
       {
               pVehInfo->nSmallImgSize=0;
       }
       //取二值化车牌图
       if (VPR_OK!=VPR_GetBinImg(nIndex, pVehInfo->lpBinImgBuf, pVehInfo->nBinImgSize, pVehInfo->nBinImgWidth, pVehInfo->nBinImgHeight))
       {
               pVehInfo->nBinImgSize=0;
               pVehInfo->nBinImgHeight=0;
               pVehInfo->nBinImgWidth=0;
       }
       if (VPR_OK!=VPR_GetPlate(nIndex, pVehInfo->nColor, pVehInfo->lpszPlate, pVehInfo->nRating))
       {
               pVehInfo->nColor=VP_COLOR_NONE;
               if (pVehInfo->lpszPlate)
               {
                       pVehInfo->lpszPlate[0]=0;
               }
       }
       return VPR_OK;
}
int VPR_GetVer(char* lpszVer, char* lpszVender, char* lpszDevType)
{
       if ((NULL==lpszVer) || NULL==lpszVender || NULL==lpszDevType)
       {
               return -1;
       }
       /*_snprintf_s(lpszVer, 20, _TRUNCATE, "Version 1.0");
       _snprintf_s(lpszVender, 50, _TRUNCATE, "北京信路威");
       _snprintf_s(lpszDevType, 20, _TRUNCATE, "");*/

       strncpy(lpszVer,"Version 1.0",20);
       strncpy(lpszVender,"北京信路威",50);
       strncpy(lpszDevType,"",20);
       return 0;
}

int VPR_RegisterMessage(QObject *hHandle, quint32 nDataMsgID)
{
    if(hHandle == NULL)
    {
        return -1;
    }
    m_hParentVPRHandle = hHandle;
    m_nDataMsgID = nDataMsgID;
    return VPR_OK;
}
