#include "vprlib.h"
#include <QString>
#include <QDebug>
#include <QCoreApplication>
#include <QTextCodec>
XLW_VehicleInfo g_VehicleInfoArray;
CAutoDetectedVehInfo Vehinfo;
quint32 m_nDataMsgID;			//	车牌识别消息标识
QObject *m_hParentVPRHandle=NULL;	//	窗口句柄

QTcpClient* getTcpClient()
{
    static QTcpClient client;
    return &client;
}

bool m_bDevOpened=false;			//  设备是否已经初始化

char* GB2312toUtf8(const char * str, int size = -1)
{
    QByteArray bytearr;
    if(size == -1)
    {
         bytearr = QTextCodec::codecForName("GB18030")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str));
    }
    else
    {
        bytearr = QTextCodec::codecForName("GB18030")->fromUnicode(QTextCodec::codecForName("UTF-8")->toUnicode(str, size));
    }
    return bytearr.data();
}
int VPR_RegisterCallback(Func_VPR_NotifyFunc NotifyFunc)
{
    // 暂不支持回调函数接口
    return VPR_ERR_NO_SUPPORT;
}


int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2)
{
    getTcpClient()->SetType(DEV_VPR);
    getTcpClient()->SetFilename(QString("./LogVpr.log"));
    if(getTcpClient()->isConnected())
    {

        m_bDevOpened=true;
        return VPR_OK;
    }
    else
    {
        if(getTcpClient()->ConnecttoServer(lpszConnPara1))
        {
            getTcpClient()->SendInit();
            getTcpClient()->SetBack(ReadStr);

            m_bDevOpened=true;
            return VPR_OK;
        }
        return VPR_ERR_UNKNOWN;
    }
}
int VPR_Disconnect(void)
{
    if (!m_bDevOpened)
    {
        return VPR_OK;
    }
    getTcpClient()->Disconnected();
    m_bDevOpened=false;
    return VPR_OK;
}

int VPR_Capture(void)
{
    m_bDevOpened = getTcpClient()->isConnected();
    if (!m_bDevOpened)
    {
        return VPR_ERR_UNKNOWN;
    }
    return VPR_OK;
}
int VPR_GetPlate(quint32 nIndex, int &nColor, char* lpszPlate, quint32 &nRating)
{
    if((g_VehicleInfoArray.m_Color < 0 )|| (g_VehicleInfoArray.m_szPlate[0] == '\0'))
    {
        return VPR_ERR_UNKNOWN;
    }
    nColor = g_VehicleInfoArray.m_Color;
    strcpy(lpszPlate,GB2312toUtf8(g_VehicleInfoArray.m_szPlate));
    qDebug()<<lpszPlate;
    nRating=99;
    return VPR_OK;
}

int VPR_GetBigImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize)
{
    qDebug() << "lib get big image" << nIndex << g_VehicleInfoArray.m_nBigImgSize << nSize;

    if (0>=g_VehicleInfoArray.m_nBigImgSize)
    {
        // 无图像
        nSize=0;
        return VPR_ERR_NO_DATA;
    }
    if (NULL==lpImgBuf || nSize<g_VehicleInfoArray.m_nBigImgSize)
    {
        // 图像大小大于缓冲区
        nSize=g_VehicleInfoArray.m_nBigImgSize;
        return VPR_ERR_RANGE;
    }
    nSize=g_VehicleInfoArray.m_nBigImgSize;
    memcpy(lpImgBuf, g_VehicleInfoArray.m_baBigImg, g_VehicleInfoArray.m_nBigImgSize);
    return VPR_OK;
}

int  VPR_GetSmallImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize)
{
    //程序中目前用不到小图
    return VPR_OK;
}

int VPR_GetBinImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize, quint32 &nWidth, quint32 &nHeight)
{
    // 暂不支持取二值化图像
    return VPR_ERR_NO_SUPPORT;
}

int VPR_GetAllVehInfo(quint32 nIndex, CAutoDetectedVehInfo *pVehInfo)
{
    //取大图
    if (VPR_OK!=VPR_GetBigImg(nIndex, pVehInfo->lpBigImgBuf, pVehInfo->nBigImgSize))
    {
        pVehInfo->nBigImgSize=0;
    }
    //获取车牌
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
    qDebug()<<"feng -- 已设置事件接收对象";
    return VPR_OK;
}
//车牌号回调函数
int onPlateNoCallBack(qint32 dwCarID, char* pcPlateNo,int nColor)
{
    qDebug()<<"识别到车牌，车辆ID="<<dwCarID<<"车牌="<<pcPlateNo;
    bool bRlt;
    bRlt=g_VehicleInfoArray.SavePlate(pcPlateNo, dwCarID , nColor);
    if(m_hParentVPRHandle == NULL)
    {
        qDebug()<<"feng -- m_hParentVPRHandle为空 -- plate";
    }
    if (bRlt)
        QCoreApplication::postEvent((QObject*)m_hParentVPRHandle, new VPREVENT(m_nDataMsgID, 0));
    return 0;
}

//车牌大图回调函数
int onBigImgCallBack(quint32 dwCarID, quint32 wSize, quint8* pbImage)
{
    qDebug()<<"车牌大图，车辆ID="<<dwCarID<<"大小="<<wSize;
    bool bRlt;
    bRlt=g_VehicleInfoArray.SaveBigImg(dwCarID, pbImage, wSize);
    if(m_hParentVPRHandle == NULL)
    {
        qDebug()<<"feng -- m_hParentVPRHandle为空 -- Img";
    }
    if (bRlt)
        QCoreApplication::postEvent((QObject*)m_hParentVPRHandle, new VPREVENT(m_nDataMsgID, 0));
    return 0;
}
void ReadStr(QByteArray &data)
{//需要改
    int platesize,size;
    size = data.size();
    Vehinfo.nColor = data.left(1).toInt();
    platesize = Char2Dec(data.right(size-1).left(4).data());
    Vehinfo.lpszPlate = data.right(size-5).left(platesize).data();
    Vehinfo.nBigImgSize = Char2Dec(data.right(size-5-platesize).left(4).data());
    Vehinfo.lpBigImgBuf = (unsigned char*)data.right(size-9-platesize).data();
    onPlateNoCallBack(0,Vehinfo.lpszPlate,Vehinfo.nColor);
    onBigImgCallBack(0,Vehinfo.nBigImgSize,Vehinfo.lpBigImgBuf);
    qDebug()<<"Plate::"<<Vehinfo.lpszPlate;
    qDebug()<<"color::"<<Vehinfo.nColor;
    qDebug()<<"ImgSize::  "<<Vehinfo.nBigImgSize;
    Vehinfo.nColor = -1;
    Vehinfo.lpszPlate = (char*)malloc(sizeof(char));
    strcpy(Vehinfo.lpszPlate,"");
    Vehinfo.nRating = 0;
    Vehinfo.nBigImgSize = 0;
}
