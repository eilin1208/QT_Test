#include "vprarm.h"
#include <QImage>
#include <QStringList>
#include "CommonModule/LaneType.h"
#include "CommonModule/laneutils_new.h"
#include "CommonModule/vehplatefunc.h"
#define BIG_IMG_BUF_SIZE (200*1024)
#define SMALL_IMG_BUF_SIZE (20*1024)
#define BIN_IMG_BUF_SIZE (500)

/*-----------------------------------------------------------------------------
//注册窗口句柄和自定义消息
//消息参数定义：
//PostMessage(
//			HWND,     		// 目标窗口句柄，即hHandle参数
//			WM_XXX,			// 自定义消息，即nDataMsgID参数
//			WPARAM,     	// 返回的数据序号，用来保证车牌、图像的一致性
//			LPARAM      	// 未使用，为0
//			);
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_RegisterMessage)(QObject* hHandle, quint32 nDataMsgID);

//车牌识别事件回调函数定义。
typedef void (*Func_VPR_NotifyFunc)(quint32 uIndex);
//注册车牌识别事件通知回调函数。
typedef int (*Func_VPR_RegisterCallback)(Func_VPR_NotifyFunc NotifyFunc);

/*-----------------------------------------------------------------------------
//连接车牌识别系统。
//参数：
//	lpszConnPara1：连接参数1
//	lpszConnPara2：连接参数2
//返回：
//	0-成功；
//	其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_Connect)(char * lpszConnPara1, char* lpszConnPara2);

/*-----------------------------------------------------------------------------
//断开与车牌识别系统的连接
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_Disconnect)(void);

/*-----------------------------------------------------------------------------
//主动触发车牌识别系统抓怕图像并识别车牌，函数为非阻塞方式，调用后立即返回，
  抓拍识别工作完成并准备好数据后通过消息或回调的方式通知上层应用。
//参数：无
//返回：
//0-成功；
//其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_Capture)(void);

/*-----------------------------------------------------------------------------
//获取指定序号车牌颜色、车牌号。
//车牌颜色定义为：0-蓝牌；1-黄牌；2-黑牌；3-白牌；-1-无颜色
//车牌格式例如：“鲁A12345”
//参数：
//nIndex：数据序号；
//nColor: 车牌颜色代码；
//lpszPlate：车牌号，如未识别出车牌或无车牌输出零长度字符串；
//nRating：车牌的可信度，为整数，例如nRating=85，表示可信度为85%。
//返回：
//0-成功；
//其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetPlate)(quint32 nIndex, int &nColor, char * lpszPlate, quint32 &nRating);

/*-----------------------------------------------------------------------------
//获取指定序号的车辆大图。
//参数：
//nIndex：数据序号；
//lpImgBuf：图像数据缓冲区，缓冲区大小必须≥200K;
//nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
//如图像大小大于缓冲区大小，也通过该参数输出图像实际大小。
//返回：
//0-成功；
//1-图像大小大于缓冲区
//2-无图像
//其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetBigImg)(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize);

/*-----------------------------------------------------------------------------
获取指定序号的车牌小图。
参数：
nIndex：数据序号；
lpImgBuf：图像数据缓冲区，缓冲区大小必须≥20K;
nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
如图像大小大于缓冲区大小，也通过该参数输出图像实际大小。
返回：
0-成功；
1-图像大小大于缓冲区
2-无图像
其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetSmallImg)(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize);

/*-----------------------------------------------------------------------------
获取指定序号的车牌二值图。
参数：
nIndex：数据序号；
lpImgBuf：图像数据缓冲区，缓冲区大小必须≥280字节;
nWeidth：输出的二值图像宽度；
nHeight：输出的二值图像高度；
nSize： 输入时为上层应用分配的缓冲区大小，输出时为图像实际大小；
如图像大小大于缓冲区大小，也通过该参数输出图像实际大小。
返回：
0-成功；
1-图像大小大于缓冲区
2-无图像
其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetBinImg)(quint32 nIndex, quint8* lpImgBuf, quint32 &nSize, quint32 &nWidth, quint32 &nHeight);

#pragma pack(push, 1)
typedef struct
{
    int nColor;					//	车牌颜色代码
    char* lpszPlate;			//	车牌号
    quint32 nRating;				//	车牌的可信度
    quint8* lpBigImgBuf;			//	车辆大图图像数据缓冲区
    quint32 nBigImgSize;			//	车辆大图图像大小
    quint8* lpSmallImgBuf;		//	车牌小图图像数据缓冲区
    quint32 nSmallImgSize;			//	车牌小图图像大小
    quint8* lpBinImgBuf;			//	二值化图像数据缓冲区
    quint32 nBinImgSize;			//	二值化图像数据大小
    quint32 nBinImgWidth;			//	二值化图像宽度
    quint32 nBinImgHeight;			//	二值化图像高度
}CAutoDetectedVehInfo;
#pragma pack(pop)

/*-----------------------------------------------------------------------------
获取车牌、大图、车牌小图、二值化图等数据。
参数：
nIndex：数据序号；
PAutoDetectedVehInfo: 车牌识别结果结构
返回：
0-成功；
其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetAllVehInfo)(quint32 nIndex, CAutoDetectedVehInfo *pVehInfo);

/*-----------------------------------------------------------------------------
获取动态链接库的版本号和厂家信息。
参数：
lpszVer：动态链接库版本号，至少分配20个字节；
lpszVerder：厂商名称，至少分配50字节；
lpszDevType：设备型号，如果非空指针，可以写入50字节；
返回：
0-成功；
其它-失败；
-----------------------------------------------------------------------------*/
typedef int (*Func_VPR_GetVer)(char* lpszVer, char* lpszVender, char* lpszDevType);


//声明函数类型的变量并初始化
Func_VPR_RegisterMessage VPR_RegisterMessage = NULL;
Func_VPR_RegisterCallback VPR_RegisterCallback = NULL;
Func_VPR_Connect VPR_Connect = NULL;
Func_VPR_Disconnect VPR_Disconnect = NULL;
Func_VPR_Capture VPR_Capture = NULL;
Func_VPR_GetPlate VPR_GetPlate = NULL;
Func_VPR_GetBigImg VPR_GetBigImg = NULL;
Func_VPR_GetSmallImg VPR_GetSmallImg = NULL;
Func_VPR_GetBinImg VPR_GetBinImg = NULL;
Func_VPR_GetAllVehInfo VPR_GetAllVehInfo = NULL;
Func_VPR_GetVer VPR_GetVer = NULL;


VPRArm::VPRArm(const QString& sName,QObject *parent) :
    DevBaseInterface(sName,parent)
{
    m_nCurrentIndex = 0;
    m_bManualCapture = false;
    for(int i = 0; i < 3; i++)
    {
        m_VPRResultArray[i].AllocateMem();
        m_VPRResultArray[i].ClearResult();
    }
}

void VPRArm::SetEventProcWnd(QObject *handle, quint32 uMsg)
{
    m_hEventProcWnd = handle;
    m_uMsg = uMsg;
    if((!m_bDriverLoaded) || (!VPR_RegisterMessage))
    {
        return;
    }
    int nRet = VPR_RegisterMessage(m_hEventProcWnd, uMsg);
    if(nRet != 0)
    {
        LogMsg("lane", tr("注册车牌识别消息失败，Error Code=%1").arg(nRet));
    }
}

void CVPRResult::ClearResult()
{
    nColor=-1;
    nRating=0;
    nBigImgSize=0;
    nSmallImgSize=0;
    nBinImgSize=0;
    nBinImgWidth=0;
    nBinImgHeight=0;
}

bool CVPRResult::AllocateMem()
{
    ReleaseMem();

    lpBigImgBuf=new unsigned char[BIG_IMG_BUF_SIZE];
    nBigImgSize=0;

    lpSmallImgBuf=new unsigned char[SMALL_IMG_BUF_SIZE];
    nSmallImgSize=0;

    lpBinImgBuf=new unsigned char[BIN_IMG_BUF_SIZE];
    nBinImgSize=0;

    return true;
}

void CVPRResult::ReleaseMem()
{
    if (lpBigImgBuf)
    {
        delete [] lpBigImgBuf;
        lpBigImgBuf=NULL;
    }
    if (lpSmallImgBuf)
    {
        delete [] lpSmallImgBuf;
        lpSmallImgBuf=NULL;
    }
    if (lpBinImgBuf)
    {
        delete [] lpBinImgBuf;
        lpBinImgBuf=NULL;
    }
}

bool CVPRResult::SaveBigImgAsFile(const char *szFileName) const
{
    QImage image;
    QString fileName = QString::fromLocal8Bit(szFileName);
    image.load(fileName);
    fileName = fileName.split(".").at(0);
    fileName.append(".jpg");
    image.save(fileName, "JPG");
    return true;
}

bool CVPRResult::HasBigImg() const
{
    ////qDebug() << (int)lpBigImgBuf << nBigImgSize << BIG_IMG_BUF_SIZE;
    return (NULL!=lpBigImgBuf) && (nBigImgSize>0) && (nBigImgSize<=BIG_IMG_BUF_SIZE);
}

bool CVPRResult::HasSmallImg() const
{
    return (NULL!=lpSmallImgBuf) && (nSmallImgSize>0) && (nSmallImgSize<=SMALL_IMG_BUF_SIZE);
}

bool CVPRResult::HasBinImg() const
{
    return (NULL!=lpBinImgBuf) && (nBinImgSize>0) && (nBinImgSize<=BIN_IMG_BUF_SIZE);
}

bool VPRArm::Capture()
{
    if(!m_bDriverLoaded)
    {
        return false;
    }

    return VPR_Capture() == 0;
}

bool VPRArm::SaveVPRResult(int nIndex)
{
    int n = 0;
    int nRet = 0;
    CAutoDetectedVehInfo vehInfo;
    char szVehPlateModified[MAX_VEHPLATE_LEN];
    char szVehPlate[128];
    if(!m_bDriverLoaded)
    {
        return false;
    }

    if(nIndex < 0)
    {
        return false;
    }
    n = nIndex % 3;
    CVPRResult &vprResult = m_VPRResultArray[n];
    vprResult.ClearResult();

    qMemSet(&vehInfo, 0, sizeof(vehInfo));
    qMemSet(szVehPlateModified,0, sizeof(szVehPlateModified));
    qMemSet(szVehPlate, 0, 128);

    vehInfo.lpszPlate = szVehPlate;
    vehInfo.lpBigImgBuf = vprResult.lpBigImgBuf;
    vehInfo.nBigImgSize = BIG_IMG_BUF_SIZE;
    vehInfo.lpSmallImgBuf = vprResult.lpSmallImgBuf;
    vehInfo.nSmallImgSize = SMALL_IMG_BUF_SIZE;
    vehInfo.lpBinImgBuf = vprResult.lpBinImgBuf;
    vehInfo.nBinImgSize = BIN_IMG_BUF_SIZE;

    nRet = VPR_GetAllVehInfo(nIndex, &vehInfo);
    ////qDebug() << vehInfo.nBigImgSize;
    if(nRet != 0)
    {
        LogMsg("lane", tr("GetAllVehInfo调用失败，返回%1").arg(nRet));
    }
    LogMsg("lane",tr("veh plate result:%1").arg(GB2312toUtf8(szVehPlate)));
    //去除车牌特殊字符函数
    RemovePlateSpecChar(szVehPlateModified, MAX_VEHPLATE_LEN+1, szVehPlate);

    vprResult.nIndex = nIndex;
    vprResult.nColor = vehInfo.nColor;
    vprResult.sVehPlate = GB2312toUtf8(szVehPlateModified);
    vprResult.nRating = vehInfo.nRating;
    vprResult.nBigImgSize = vehInfo.nBigImgSize;
    vprResult.nSmallImgSize = vehInfo.nSmallImgSize;
    vprResult.nBinImgSize = vehInfo.nBinImgSize;
    vprResult.nBinImgWidth = vehInfo.nBinImgWidth;
    vprResult.nBinImgHeight = vehInfo.nBinImgHeight;
    m_nCurrentIndex = nIndex;
    return true;
}

const CVPRResult * VPRArm::GetVPRResult(int nIndex)
{
    int n = 0;
    if(nIndex < 0)
    {
        return NULL;
    }
    n = nIndex % 3;
    CVPRResult &vprResult = m_VPRResultArray[n];
    ////qDebug() << "big image size" <<m_VPRResultArray[n].nBigImgSize;
    if(vprResult.nIndex == nIndex)
    {
        return &vprResult;
    }
    return NULL;
}

const CVPRResult * VPRArm::GetCurrentResult()
{

    return GetVPRResult(m_nCurrentIndex);
}

void VPRArm::ClearVPRResult()
{
    m_nCurrentIndex = -1;
}

bool VPRArm::GetVersion(QString &sVersion, QString &sVender)
{
    char szVersion[256] = "";
    char szVender[256] = "";
    char szDevType[256] = "";
    int nRet = 0;
    if(!m_bDriverLoaded)
    {
        return false;
    }
    nRet = VPR_GetVer(szVersion, szVender, szDevType);
    if(nRet != 0)
    {
        LogMsg("lane", tr("VPR_GetVer调用失败，返回%1").arg(nRet));
    }
    sVersion = GB2312toUtf8(szVersion);
    sVender = GB2312toUtf8(szVender);
    return true;
}

bool VPRArm::IsManualCapture()
{
    return m_bManualCapture;
}

void VPRArm::SetManualCapture(bool bManualCapture)
{
    m_bManualCapture = bManualCapture;
}

bool VPRArm::GetPlate(quint32 nIndex, VPRArm::VEH_PLATE &vp)
{
    int nColor = 0;
    int nRet = 0;
    char buf[256];
    if(!m_bDriverLoaded)
    {
        return false;
    }

    qMemSet(buf, 0, sizeof(buf));
    nRet = VPR_GetPlate(nIndex, nColor, buf, vp.nRating);
    if(nRet != 0)
    {
        LogMsg("lane", tr("GetPlate调用失败, 返回%1").arg(nRet));
        return false;
    }
    switch(nColor)
    {
    case 0:
        vp.nColor = VP_COLOR_BLUE;
        break;
    case 1:
        vp.nColor = VP_COLOR_YELLOW;
        break;
    case 2:
        vp.nColor = VP_COLOR_BLACK;
        break;
    case 3:
        vp.nColor = VP_COLOR_WHITE;
        break;
    default:
        vp.nColor = VP_COLOR_NONE;
        break;
    }

    vp.sVehPlate = GB2312toUtf8(buf);
    return true;
}

bool VPRArm::GetBigImage(quint32 nIndex, const QString &sImageName)
{

    quint32 nBufferLen;
    quint8 lpBuffer[BIG_IMG_BUF_SIZE];
    int nRet = 0;
    nBufferLen = BIG_IMG_BUF_SIZE;
    if(!m_bDriverLoaded)
    {
        return false;
    }

    nRet = VPR_GetBigImg(nIndex, lpBuffer, nBufferLen);
    if(nRet != 0)
    {
        LogMsg("lane", tr("GetBigImage调用失败，返回%1，bufferLen=%2").arg(nRet).arg(nBufferLen));
        return false;
    }

    return WriteBufToFile(sImageName, (const char*)lpBuffer, nBufferLen);
}

bool VPRArm::GetSmallImage(quint32 nIndex, const QString &sImageName)
{
    quint32 nBufferLen = SMALL_IMG_BUF_SIZE;
    quint8 lpBuffer[SMALL_IMG_BUF_SIZE];
    int nRet = 0;
    if(!m_bDriverLoaded)
    {
        return false;
    }
    nRet = VPR_GetSmallImg(nIndex, lpBuffer, nBufferLen);
    if(nRet != 0)
    {
        LogMsg("lane", tr("GetSmallImg调用失败，返回%1，BufferLen=%2").arg(nRet).arg(nBufferLen));
        return false;
    }

    return WriteBufToFile(sImageName, (const char*)lpBuffer, nBufferLen);
}

bool VPRArm::GetBinImage(quint32 nIndex, const QString &sImageName, quint32 &nWidth, quint32 &nHeight)
{
    quint32 nBufferLen = BIN_IMG_BUF_SIZE;
    quint8 lpBuffer[BIN_IMG_BUF_SIZE];
    int nRet = 0;
    if(!m_bDriverLoaded)
    {
        return false;
    }
    nRet = VPR_GetBinImg(nIndex, lpBuffer, nBufferLen, nWidth, nHeight);
    if(nRet != 0)
    {
        LogMsg("lane", tr("GetbinImg调用失败，返回%1，BufferLen=%2").arg(nRet).arg(nBufferLen));
        return false;
    }
    return WriteBufToFile(sImageName, (const char*)lpBuffer, nBufferLen);
}

VPRArm * getVpr()
{
    static VPRArm vpr("VPR");
    return &vpr;
}

bool VPRArm::LoadFunction()
{
    if(m_bDriverLoaded)
    {
        return true;
    }
    VPR_RegisterMessage = (Func_VPR_RegisterMessage)m_qLibrary.resolve("VPR_RegisterMessage");
    if(VPR_RegisterMessage == NULL)
    {
        qDebug() << "reg message null";
    }
    VPR_RegisterCallback = (Func_VPR_RegisterCallback)m_qLibrary.resolve("VPR_RegisterCallback");
    VPR_Connect = (Func_VPR_Connect)m_qLibrary.resolve("VPR_Connect");
    VPR_Disconnect = (Func_VPR_Disconnect)m_qLibrary.resolve("VPR_Disconnect");
    VPR_Capture = (Func_VPR_Capture)m_qLibrary.resolve("VPR_Capture");
    VPR_GetPlate = (Func_VPR_GetPlate)m_qLibrary.resolve("VPR_GetPlate");
    VPR_GetBigImg = (Func_VPR_GetBigImg)m_qLibrary.resolve("VPR_GetBigImg");
    VPR_GetSmallImg = (Func_VPR_GetSmallImg)m_qLibrary.resolve("VPR_GetSmallImg");
    VPR_GetBinImg = (Func_VPR_GetBinImg)m_qLibrary.resolve("VPR_GetBinImg");
    VPR_GetAllVehInfo = (Func_VPR_GetAllVehInfo)m_qLibrary.resolve("VPR_GetAllVehInfo");
    VPR_GetVer = (Func_VPR_GetVer)m_qLibrary.resolve("VPR_GetVer");
    if ((NULL==VPR_RegisterMessage) ||
            (NULL==VPR_RegisterCallback) ||
            (NULL==VPR_Connect) ||
            (NULL==VPR_Disconnect) ||
            (NULL==VPR_Capture) ||
            (NULL==VPR_GetPlate) ||
            (NULL==VPR_GetBigImg) ||
            (NULL==VPR_GetSmallImg) ||
            (NULL==VPR_GetBinImg) ||
            (NULL==VPR_GetAllVehInfo) ||
            (NULL==VPR_GetVer))
    {
        qDebug() << "vpr lib load failed";
        LogMsg("lane",tr("获取车牌识别动态库[%1]中的一个或多个函数错误！").arg(m_sLibName));
        return false;
    }
    m_bDriverLoaded = true;
    return true;
}

bool VPRArm::ReleaseFunction()
{
    // 标记动态库未加载
    m_bDriverLoaded=false;
    // 函数指针置为NULL
    VPR_RegisterMessage = NULL;
    VPR_RegisterCallback = NULL;
    VPR_Connect = NULL;
    VPR_Disconnect = NULL;
    VPR_Capture = NULL;
    VPR_GetPlate = NULL;
    VPR_GetBigImg = NULL;
    VPR_GetSmallImg = NULL;
    VPR_GetBinImg = NULL;
    VPR_GetAllVehInfo = NULL;
    VPR_GetVer = NULL;
    return true;
}

bool VPRArm::Close()
{
    if(!m_bInited)
    {
        if(m_bDriverLoaded && VPR_Disconnect != NULL)
        {
            VPR_Disconnect();
        }
    }
    return true;
}

bool VPRArm::Init()
{
    const int MAX_LENGTH = 128;
    char szVersion[MAX_LENGTH] = "";
    char szVender[MAX_LENGTH] = "";
    char szDevType[MAX_LENGTH] = "";

    //避免重复初始化
    if(m_bInited)
    {
        Close();
    }
    qDebug() << "vpr init" << m_sConStr1 << m_sConStr2;
    int nRet = VPR_Connect(m_sConStr1.toLocal8Bit().data(), m_sConStr2.toLocal8Bit().data());
    if(nRet != 0)
    {
        return false;
    }

    nRet = VPR_GetVer(szVersion, szVender, szDevType);
    if(nRet == 0)
    {
        LogMsg("lane", tr("打开车牌识别设备成功，动态库版本[%1]，厂商型号[%2 %3]").arg(szVersion).arg(szVender).arg(szDevType));
    }
    else
    {
        LogMsg("lane", tr("打开车牌识别设备成功，但获取动态库版本信息失败，Error Code=%1").arg(nRet));
    }
    return true;
}
