#include "devicefactory.h"
#include "CommonModule/LaneType.h"
#include "CommonModule/ilogmsg.h"

DeviceFactory::DeviceFactory(QObject *parent) :
    QObject(parent)
{

}

void DeviceFactory::InitAllDevice(int nLaneType)
{
    //TODO：4个卡读写器只用一个
    if(nLaneType == LaneType_AutoCard)
    {
        m_mapDevice[DEV_CardReader1] = new QCardReader("CardReader1");
        m_mapDevice[DEV_CardReader2] = new QCardReader("CardReader2");
        m_mapDevice[DEV_CardReader3] = new QCardReader("CardReader3");
        m_mapDevice[DEV_CardReader4] = new QCardReader("CardReader4");
        m_mapDevice[DEV_CardManageMachine] = new CardMgrArm("CardMgr");
    }
    else
    {
        m_mapDevice[DEV_CardReader1] = new QCardReader("CardReader1");
    }
    m_mapDevice[DEV_IOCard] = new IOBoardArm("IOCard");
    m_mapDevice[DEV_VehPlateRecognizer] = new VPRArm("VPR");
    m_mapDevice[DEV_VideoCard] = new VideoCard_ARM("VideoCard");
    m_mapDevice[DEV_VDM] = new VDMArm("VDM");
    if(nLaneType == LaneType_MtcOut)
    {
        m_mapDevice[DEV_FareDisplayer] = new FareDisplayArm("FD");
        m_mapDevice[DEV_SmallPrinter] = new PrinterArm("Printer");
        m_mapDevice[DEV_WEIGHT] = new TWtDev("WeightDev");
    }
}
bool DeviceFactory::OpenAllDevice()
{
    DevBaseInterface* tmpDev = NULL;
    bool ok = true;
    QMap< DEVNO, DevBaseInterface* >::iterator it = m_mapDevice.begin();
    for(; it != m_mapDevice.end(); it++)
    {
        tmpDev = it.value();
        bool ret = tmpDev->StartDev();
        if(!ret)
        {
            ok = false;
        }
        //显示到日志
        LogMsg("", getDeviceName(it.key()) + (ok ? tr("初始化成功") : tr("初始化失败")), true);
    }
    return ok;
}

void DeviceFactory::ReleaseAllDevice()
{
    DevBaseInterface* tmpDev = NULL;
    QMap< DEVNO, DevBaseInterface* >::iterator it = m_mapDevice.begin();
    for(; it != m_mapDevice.end(); it++)
    {
        tmpDev = it.value();
        tmpDev->StopDev();
        delete tmpDev;
        it.value() = NULL;
    }
}
DeviceFactory* getDeviceFactory()
{
    static DeviceFactory df;
    return &df;
}

QString getDeviceName(DEVNO nDev)
{
    switch(nDev)
    {
    case DEV_CardReader1:
        return QObject::tr("卡读写器1");
    case DEV_CardReader2:
        return QObject::tr("卡读写器2");
    case DEV_CardReader3:
        return QObject::tr("卡读写器3");
    case DEV_CardReader4:
        return QObject::tr("卡读写器4");
    case DEV_IOCard:
        return QObject::tr("IO模块");
    case DEV_SmallPrinter:
        return QObject::tr("打印机");
    case DEV_CardManageMachine:
        return QObject::tr("卡机");
    case DEV_VideoCard:
        return QObject::tr("车道摄像机");
    case DEV_VehPlateRecognizer:
        return QObject::tr("车牌识别");
    case DEV_FareDisplayer:
        return QObject::tr("费显");
    case DEV_VDM:
        return QObject::tr("字符叠加");
    case DEV_WEIGHT:
        return QObject::tr("计重");
    default:
        return "";
    }
}
