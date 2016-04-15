#ifndef VIDEOCARD_ARM_H
#define VIDEOCARD_ARM_H

#include <QObject>
#include <QWidget>
#include <QLibrary>
#include <QDateTime>
#include "BaseInterface/devbaseinterface.h"
#include "video_lib.h"
//函数指针
typedef int (*myVideoInit)(int,int,int,int);
typedef int (*myVideoStart)(int,int,int,int);
typedef int (*myVideoInitCapture)(char * fileName, int quality);
class VideoCard_ARM : public DevBaseInterface
{
    Q_OBJECT
public:
    explicit VideoCard_ARM(const QString& sName,QObject *parent = 0);

public:
    //设置视频显示页面
    bool SetVideoUi(QWidget *yourWidget);
    // 启动设备
    virtual bool StartDev();
    virtual bool InitVideoCap();
public:
    // 显示
    bool Display();
    //预先抓拍
    bool Prepare();
    //判断是否有抓拍图片
    bool HasCapturedPic();
    // 取预抓拍图像文件名
    bool GetPreparedBmpFileName(QString& sFileName);
    // 将预抓拍图像以JPG格式保存为指定文件
    bool SaveAsJpgFile(const QString& sFileName);

    QString getFileName();
    QDateTime getFileTime();

protected:
    virtual bool LoadFunction();
    // 释放动态库
    virtual bool ReleaseFunction();
    // 初始化
    virtual bool Init();
    // 关闭
    virtual bool Close();

    bool Capture(const QString& sFileName);

    //日志显示
    void ShowLog(QString Message,int ShowStyle);


private:

    //日志显示方式(1--Qdebug, 2--LOG)
    int m_LogStyle;
private:
    //视频初始化函数指针
    myVideoInit pFunVideoInit;

    //视频打开的函数指针
    myVideoStart pFunVodeoStart;
    //视频抓拍函数指针
    myVideoInitCapture pFunVideoCap;
    //视频要显示的页面指针
    QWidget *pShowWidget;
    //是否完成预先抓拍
    bool m_bHasPreparedImg;
    //抓怕时间
    QDateTime m_captureTime;
    QString m_sFileName;
};


#endif // VIDEOCARD_ARM_H
