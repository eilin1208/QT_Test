#ifndef VPRARMLIB_UC_H
#define VPRARMLIB_UC_H

#include "VprArmLib_UC_global.h"
#include <QObject>
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
        unsigned char* lpSmallImgBuf;		//	车牌小图图像数据缓冲区
        quint32 nSmallImgSize;			//	车牌小图图像大小
        unsigned char* lpBinImgBuf;			//	二值化图像数据缓冲区
        quint32 nBinImgSize;			//	二值化图像数据大小
        quint32 nBinImgWidth;			//	二值化图像宽度
        quint32 nBinImgHeight;			//	二值化图像高度
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
class VPREVENT : public QEvent
{
public:
    explicit VPREVENT(quint32 dataMsgID, int nIndex) : QEvent((Type)(2002)), m_nDataMsgId(dataMsgID), m_nIndex(nIndex)
    {

    }
    quint32 m_nDataMsgId;
    int m_nIndex;
};


#endif // VPRARMLIB_UC_H
