#include <QDebug>
#include "videocardarm.h"
#include <QFile>
#include <QDebug>
#include <QTime>
char * g_szDefaultName = "/CapTmp.jpg";
VideoCard_ARM::VideoCard_ARM(const QString& sName,QObject *parent) :
    DevBaseInterface(sName,parent)
{
    //数据初始化
    pFunVideoInit=NULL;
    pFunVodeoStart=NULL;
    pFunVideoCap = NULL;
    m_bDriverLoaded=false;
    m_bInited=false;
    m_LogStyle=1;
    pShowWidget=NULL;
    m_bHasPreparedImg = false;
}

void VideoCard_ARM::ShowLog(QString Message, int ShowStyle)
{
    switch(ShowStyle)
    {
    case 1:
        //QDebug()<<Message;
        break;
    default:
        /*----------------------在这里进行日志输出----------------------*/
        break;
    }
}

bool VideoCard_ARM::StartDev()
{
    bool ret = DevBaseInterface::StartDev();
    if(!ret)
    {
        return false;
    }
    //    //打开设备
    //    if(!Display())
    //    {
    //        LogMsg("lane", tr("视频采集卡显示失败,errstring=[%1]").arg(m_sErrString));
    //        return false;
    //    }
    m_bIsOpen =true;
    return true;
}

bool VideoCard_ARM::InitVideoCap()
{
    //查看类库是否初始化
    if(!m_bDriverLoaded)
    {
        //QDebug() << tr("视频抓拍类库加载失败");
        return false;
    }
    //查看页面指针是否已经复制
    if(pShowWidget==NULL)
    {
        m_sErrString=tr("the UI Ponint is Wrong");
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }

    //获取页面的位置
    int m_posX=pShowWidget->mapToGlobal(pShowWidget->pos()).x() + 3;
    int m_posY=pShowWidget->mapToGlobal(pShowWidget->pos()).y();
    if(m_posY > 110)
    {
        m_posY -= 28;
    }
    //qDebug() <<"x" << m_posX << "y" << m_posY;
    //获取页面显示大小
    int m_hight=pShowWidget->size().height() - 10;
    int m_width=pShowWidget->size().width() - 10;
    //qDebug() << "hight" << m_hight << "width" << m_width;

    //视频初始化
    int ret=pFunVideoInit(m_posX,m_posY,m_width,m_hight);
    //int ret = pFunVideoInit(200,200,400,300);
    if(ret<0)
    {
        m_sErrString=tr("Video Init Failure , Error code：").arg(ret);
        m_nErrCode=ret;
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }

    //记录日志
    m_sErrString=tr("Video Init Success");
    ShowLog(m_sErrString,m_LogStyle);

    m_bInited=true;
    return true;
}

bool VideoCard_ARM::Display()
{
    //查看页面指针是否已经复制
    if(pShowWidget==NULL)
    {
        m_sErrString=tr("the UI Ponint is Wrong");
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }

    if(!m_bDriverLoaded)
    {
        m_sErrString = tr("The Lib have not Load!");
        return false;
    }
    //获取页面的位置
    int m_posX=pShowWidget->mapToGlobal(pShowWidget->pos()).x();
    int m_posY=pShowWidget->mapToGlobal(pShowWidget->pos()).y();
    //qDebug() <<"x" << m_posX << "y" << m_posY;
    //获取页面显示大小
    int m_hight=pShowWidget->size().height() - 10;
    int m_width=pShowWidget->size().width() - 10;
    //qDebug() << "hight" << m_hight << "width" << m_width;
    //视频初始化
    int ret=pFunVodeoStart(m_posX,m_posY,m_width,m_hight);
    //int ret = pFunVodeoStart(0,0,720,576);
    if(ret<0)
    {
        m_sErrString=tr("Video Open Failure,Error Code").arg(ret);
        m_nErrCode=ret;
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }

    //记录日志
    m_sErrString=tr("Video Open Success");
    ShowLog(m_sErrString,m_LogStyle);

    return true;
}

bool VideoCard_ARM::Prepare()
{
    m_bHasPreparedImg = false;
    if(!m_bInited)
    {
        return false;
    }
    bool bRet = Capture(g_szDefaultName);
    if(bRet)
    {
        m_bHasPreparedImg = true;
        m_captureTime = QDateTime::currentDateTime();
        return true;
    }
    else
    {
        LogMsg("lane", tr("预抓怕保存至文件[%1]失败").arg(g_szDefaultName));
        return false;
    }
}

bool VideoCard_ARM::Close()
{

    /*----------------目前给的类库里面没有提供关闭接口---------------*/
    return true;
}

bool VideoCard_ARM::SetVideoUi(QWidget *yourWidget)
{
    if(!yourWidget)
    {
        m_sErrString=tr("你传入了一个空的页面指针");
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }
    pShowWidget=yourWidget;
    return true;
}

bool VideoCard_ARM::ReleaseFunction()
{
    pFunVideoInit = NULL;
    pFunVodeoStart = NULL;
    pFunVideoCap = NULL;
    return true;
}

bool VideoCard_ARM::Init()
{
    return true;
}

bool VideoCard_ARM::LoadFunction()
{
    //QDebug() << m_sLibName;
    //导出动态库的方法
    pFunVideoInit=(myVideoInit)m_qLibrary.resolve("Video_init");
    pFunVodeoStart=(myVideoStart)m_qLibrary.resolve("SetVideo_Win");
    pFunVideoCap = (myVideoInitCapture)m_qLibrary.resolve("VideoSaveJPG");
    //导出结果判断
    if(pFunVideoInit==NULL
            ||pFunVodeoStart==NULL
            ||pFunVideoCap == NULL)
    {
        m_sErrString=m_qLibrary.errorString();
        ShowLog(m_sErrString,m_LogStyle);
        return false;
    }
    return true;
}

bool VideoCard_ARM::Capture(const QString &sFileName)
{
    if(!m_bDriverLoaded)
    {
        m_sErrString = tr("The Lib have not Load!");
        return false;
    }

    int ret = pFunVideoCap(sFileName.toLocal8Bit().data(), 90);
    if(ret < 0)
    {
        m_nErrCode = ret;
        m_sErrString = tr("Capture image failed, error code is %1").arg(ret);
        return false;
    }
    return true;
}

bool VideoCard_ARM::SaveAsJpgFile(const QString &sFileName)
{

    QPixmap image;
    if(image.load(g_szDefaultName))
    {
        if(image.save(sFileName))
        {
            //liujian 图像转存完毕后立即删除,防止下次被误传
            QFile file(g_szDefaultName);
            file.remove();
            return true;
        }
    }
    LogMsg("lane", tr("抓拍图像保存失败, 默认路径[%1]，存放路径[%2]").arg(g_szDefaultName).arg(sFileName));

    return false;
}

bool VideoCard_ARM::GetPreparedBmpFileName(QString &sFileName)
{
    if(m_bHasPreparedImg)
    {
        sFileName = g_szDefaultName;
        return true;
    }
    else
    {
        return false;
    }
}

bool VideoCard_ARM::HasCapturedPic()
{
    return m_bHasPreparedImg;
}

QString VideoCard_ARM::getFileName()
{
    return g_szDefaultName;
}

QDateTime VideoCard_ARM::getFileTime()
{
    return m_captureTime;
}
