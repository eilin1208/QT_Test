#ifndef VPRLIB_H
#define VPRLIB_H
#include "VprLib_global.h"
#include "TcpClient/qtcpclient.h"
#include <QEvent>
#define	VPR_OK				0		// 成功
#define VPR_ERR_RANGE		1		// 图像大小大于缓冲区
#define	VPR_ERR_NO_DATA		2		// 无图像
#define VPR_ERR_PARAM		97		// 参数错误
#define VPR_ERR_NO_SUPPORT	98		// 不支持
#define VPR_ERR_UNKNOWN		99		// 未知错误
typedef void (*Func_VPR_NotifyFunc)(quint32 uIndex);
#pragma pack(push, 1)
typedef struct
{
        int nColor;					//	车牌颜色代码
        char* lpszPlate;			//	车牌号 - 至少分配20字节内存
        quint32 nRating;				//	车牌的可信度 - 0-100
        unsigned char* lpBigImgBuf;			//	车辆大图图像数据缓冲区
        quint32 nBigImgSize;			//	车辆大图图像大小
}CAutoDetectedVehInfo;
#pragma pack(pop)
enum VP_COLOR
{
    VP_COLOR_NONE=-1,
    VP_COLOR_BLUE=0,
    VP_COLOR_YELLOW,
    VP_COLOR_BLACK,
    VP_COLOR_WHITE
};
class VPREVENT : public QEvent
{
public:
    explicit VPREVENT(quint32 dataMsgID, int nIndex) : QEvent((Type)(dataMsgID)), m_nDataMsgId(dataMsgID), m_nIndex(nIndex)
    {

    }
    quint32 m_nDataMsgId;
    int m_nIndex;
};
class XLW_VehicleInfo
{
public:
    quint32 m_nIndex;			//	索引
    int m_Color;                        //      车牌颜色
    quint32 m_dwCarID;                  //	车辆ID
    char m_szPlate[80];                 //	车牌号
    quint8 m_baBigImg[100*1024];	//	车辆大图
    int m_nBigImgSize;			//	车辆大图实际大小
    quint8 m_baSmallImg[10*1024];	//	车辆小图（牌照区域）
    int m_nSmallImgSize;		//	车辆小图实际大小
public:
    XLW_VehicleInfo()
    {
                m_Color = -1;
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
        m_Color = -1;
        m_nIndex = nIndex;
        m_dwCarID = dwCarID;
        m_szPlate[0] = 0;
        m_nBigImgSize = 0;
        m_nSmallImgSize = 0;
    }
    // 保存车牌
    bool SavePlate(const char *szPlate, quint32 dwCarID,int nColor)
    {
        if (NULL == szPlate)
        {
            return false;
        }
        strncpy(m_szPlate,szPlate,sizeof(m_szPlate));
        m_Color = nColor;
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

};
void ReadStr(QByteArray &data);
int onPlateNoCallBack( qint32 dwCarID, char* pcPlateNo,int nColor);
int onBigImgCallBack(quint32 dwCarID, quint32 wSize, quint8* pbImage);
DLLEXPORT int VPR_RegisterMessage(QObject* hHandle, quint32 nDataMsgID);
DLLEXPORT int VPR_RegisterCallback(Func_VPR_NotifyFunc NotifyFunc) ;
DLLEXPORT int VPR_Connect(const char* lpszConnPara1, const char* lpszConnPara2) ;
DLLEXPORT int VPR_Disconnect(void) ;
DLLEXPORT int VPR_Capture(void) ;
DLLEXPORT int VPR_GetPlate(quint32 nIndex, int &nColor,  char* lpszPlate, quint32 &nRating);
DLLEXPORT int VPR_GetBigImg(quint32 nIndex, quint8 * lpImgBuf, quint32 &nSize);
DLLEXPORT int VPR_GetSmallImg(quint32 nIndex, quint8 * lpImgBuf, quint32 &nSize);
DLLEXPORT int VPR_GetBinImg(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize, quint32 &nWidth, quint32 &nHeight);
DLLEXPORT int VPR_GetAllVehInfo(quint32 nIndex, CAutoDetectedVehInfo *pVehInfo);
DLLEXPORT int VPR_GetVer(char* lpszVer, char* lpszVender, char* lpszDevType);

#endif // VPRLIB_H
