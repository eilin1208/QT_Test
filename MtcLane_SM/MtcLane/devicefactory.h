#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include <QObject>
#include "DevModule/FareDisplayArm/faredisplayarm.h"
#include "DevModule/IoBoardArm/devioboard.h"
#include "DevModule/PrinterArm/printerarm.h"
#include "DevModule/VDMArm/vdmarm.h"
#include "DevModule/VideoCardArm/videocardarm.h"
#include "DevModule/VPRArm/vprarm.h"
#include "DevModule/CardReaderArm/qcardreader.h"
#include "DevModule/WtArm/wtdevunit.h"
#include "DevModule/CardMgrArm/cardmgrarm.h"
enum DEVNO
{
    DEV_CardReader1 = 0,                //读写器1
    DEV_CardReader2 = 1,                //读写器2
    DEV_CardReader3 = 2,                //读写器3
    DEV_CardReader4 = 3,                //读写器4
    DEV_IOCard      = 4,                //IO设备
    DEV_SmallPrinter        = 5,        //票打
    DEV_CardManageMachine   = 6,       //卡机
    DEV_VideoCard           = 7,        //视频采集卡
    DEV_VehPlateRecognizer  = 8,        //车牌识别
    DEV_FareDisplayer = 9,              //费显
    DEV_VDM     = 10,                   //字符叠加
    DEV_WEIGHT = 11                     //记重设备
};
class DeviceFactory : public QObject
{
    Q_OBJECT
public:
    explicit DeviceFactory(QObject *parent = 0);

signals:

public slots:
    void InitAllDevice(int nLaneType);
    bool OpenAllDevice();
    void ReleaseAllDevice();
public:
    QCardReader* getCardReader(DEVNO nReaderId)
    {
        return (QCardReader*)m_mapDevice.value(nReaderId, NULL);
    }

    FareDisplayArm* getFD()
    {
        return (FareDisplayArm*)m_mapDevice.value(DEV_FareDisplayer, NULL);
    }
    IOBoardArm* getIOBoard()
    {
        return (IOBoardArm*)m_mapDevice.value(DEV_IOCard, NULL);
    }
    PrinterArm * getPrinter()
    {
        return (PrinterArm*)m_mapDevice.value(DEV_SmallPrinter, NULL);
    }
    VDMArm * getVDM()
    {
        return (VDMArm*)m_mapDevice.value(DEV_VDM, NULL);
    }
    VideoCard_ARM* getVideoCard()
    {
        return (VideoCard_ARM*)m_mapDevice.value(DEV_VideoCard, NULL);
    }
    VPRArm* getVpr()
    {
        return (VPRArm*)m_mapDevice.value(DEV_VehPlateRecognizer, NULL);
    }
    TWtDev* getWtDev()
    {
        return (TWtDev*)m_mapDevice.value(DEV_WEIGHT, NULL);
    }
    CardMgrArm* getCardMgr()
    {
        return (CardMgrArm*)m_mapDevice.value(DEV_CardManageMachine, NULL);
    }

    QMap< DEVNO, DevBaseInterface* >* getMapDevice()
    {
        return &m_mapDevice;
    }
private:
    QMap< DEVNO, DevBaseInterface* > m_mapDevice;
};

DeviceFactory* getDeviceFactory();
QString getDeviceName(DEVNO nDev);
#endif // DEVICEFACTORY_H
