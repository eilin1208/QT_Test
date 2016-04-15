#ifndef VPRARM_H
#define VPRARM_H
#include "BaseInterface/devbaseinterface.h"


class CVPRResult
{
public:
    int nIndex;					//	Ë÷Òý
    int nColor;					//	³µÅÆÑÕÉ«ŽúÂë
    QString sVehPlate;			//	³µÅÆºÅ
    quint32 nRating;				//	³µÅÆµÄ¿ÉÐÅ¶È
    unsigned   char* lpBigImgBuf;			//	³µÁŸŽóÍŒÍŒÏñÊýŸÝ»º³åÇø
    quint32 nBigImgSize;			//	³µÁŸŽóÍŒÍŒÏñŽóÐ¡
    unsigned   char* lpSmallImgBuf;		//	³µÅÆÐ¡ÍŒÍŒÏñÊýŸÝ»º³åÇø
    quint32 nSmallImgSize;			//	³µÅÆÐ¡ÍŒÍŒÏñŽóÐ¡
    unsigned   char* lpBinImgBuf;			//	¶þÖµ»¯ÍŒÏñÊýŸÝ»º³åÇø
    quint32 nBinImgSize;			//	¶þÖµ»¯ÍŒÏñÊýŸÝŽóÐ¡
    quint32 nBinImgWidth;			//	¶þÖµ»¯ÍŒÏñ¿í¶È
    quint32 nBinImgHeight;			//	¶þÖµ»¯ÍŒÏñžß¶È
public:
    CVPRResult(void)
    {
        nIndex=-1;
        nColor=-1;
        nRating=0;
        lpBigImgBuf=NULL;
        nBigImgSize=0;
        lpSmallImgBuf=NULL;
        nSmallImgSize=0;
        lpBinImgBuf=NULL;
        nBinImgSize=0;
        nBinImgWidth=0;
        nBinImgHeight=0;
    }
    ~CVPRResult(void)
    {
        ReleaseMem();
    }
    void ClearResult();
    bool AllocateMem();
    void ReleaseMem();
    bool SaveBigImgAsFile(const char *szFileName) const;
    bool HasBigImg() const;
    bool HasSmallImg() const;
    bool HasBinImg() const;
};


class VPRArm : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit VPRArm(const QString& sName, QObject *parent = 0);
    typedef struct tagVehPlate
    {
        QString sVehPlate;		//	³µÅÆºÅ
        int nColor;		//	ÑÕÉ«
        quint32 nRating;			//	¿ÉÐÅ¶È£¬100±íÊŸ100%
    } VEH_PLATE;
signals:

public slots:
public:
    //设置显示的窗口句柄
    void SetEventProcWnd(QObject* handle, quint32 uMsg);
    // 手动抓拍识别
    bool Capture();
    // 保存当前抓拍识别结果
    bool SaveVPRResult(int nIndex);
    // 取抓拍结果
    const CVPRResult* GetVPRResult(int nIndex);
    // 取当前抓拍结果
    const CVPRResult* GetCurrentResult();
    // 清除抓拍结果 - 应在放行车辆后调用，以免再次显示上次的抓拍结果
    void ClearVPRResult();
    // 取版本信息
    bool GetVersion(QString& sVersion, QString& sVender);
    // 是否人工触发抓拍
    bool IsManualCapture();
    // 设置是否支持主动抓拍
    void SetManualCapture(bool bManualCapture);
public:
    // 取车牌
    bool GetPlate(quint32 nIndex, VEH_PLATE& vp);
    // 将抓拍大图保存为文件
    bool GetBigImage(quint32 nIndex,const QString& sImageName);
    // 将抓拍小图保存为文件
    bool GetSmallImage(quint32 nIndex, const QString& sImageName);
    // 取二值化图像
    bool GetBinImage(quint32 nIndex,const QString& sImageName, quint32& nWidth,quint32& nHeight);
protected:
    //加载驱动
    virtual bool LoadFunction();
    //卸载驱动
    virtual bool ReleaseFunction();
    //关闭
    virtual bool Close();
    //初始化
    virtual bool Init();
private:
    //
    CVPRResult m_VPRResultArray[3];
    //
    int m_nCurrentIndex;
    bool m_bManualCapture;
    QObject* m_hEventProcWnd;
    quint32 m_uMsg;
};

VPRArm* getVpr();
class VPREVENT : public QEvent
{
public:
    explicit VPREVENT(quint32 dataMsgID, int nIndex) : QEvent((Type)(dataMsgID)), m_nDataMsgId(dataMsgID), m_nIndex(nIndex)
    {

    }
    quint32 m_nDataMsgId;
    int m_nIndex;
};
#endif // VPRARM_H
