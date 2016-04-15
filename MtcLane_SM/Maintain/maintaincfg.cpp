#include "maintaincfg.h"
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <iterator>
QString MaintainCFG::m_CfgPath;
QString MaintainCFG::m_UpdateFilePath;
QString MaintainCFG::m_UpdateFileName;
QString MaintainCFG::m_OldFilePath;
QString MaintainCFG::m_pwd;
QString MaintainCFG::m_LaneCfgName;
int MaintainCFG::m_SoftRole;
QString MaintainCFG::m_DBName;
MaintainCFG::MaintainCFG(QObject *parent) :
    QObject(parent)
{
}
MaintainCFG::~MaintainCFG()
{
    QList<cfgNode *>::iterator it;
    for(it=listCfgNode.begin();it!=listCfgNode.end();++it)
    {
        delete *it;
    }
}
void MaintainCFG::readAllcfg()
{
    //配置读取对象
    QString fileName=QApplication::instance()->applicationDirPath() + tr("/Maintain.ini");
    QSettings m_Settings(fileName,QSettings::IniFormat);

    //读取维护软件的配置文件
    QString groupName("LaneCfg");
    setCfgPath(m_Settings.value(groupName+"/cfgPath").toString());
    setLaneCfg(m_Settings.value(groupName+"/cfgName").toString());
    setUpdateFilePath(m_Settings.value(groupName+"/NewFilePath").toString());
    setUpdateFileName(m_Settings.value(groupName+"/NewFileName").toString());
    setOldFilePath(m_Settings.value(groupName+"/OldFilePath").toString());

    groupName=tr("Login");
    setPwdMD5(m_Settings.value(groupName+"/md5").toString());
    setSoftRole(m_Settings.value(groupName+"/Role").toInt());
    setDBName(m_Settings.value(groupName+"./dbname").toString());
}

QString MaintainCFG::getCfgPath()
{
    return m_CfgPath;
}

void MaintainCFG::setCfgPath(QString path)
{
    m_CfgPath=path;
}

void MaintainCFG::getCfgInfo(QTreeWidget *widget)
{
    readAllcfg();
    //item 0
    QString sGroupName = tr("LaneInfo");
    QTreeWidgetItem *item=new QTreeWidgetItem(widget);
    item->setText(0,tr("车道基本信息"));
    listCfgData.append(item);

    //item 01
    addNextLevelNodeToList(item
                           ,tr("车道类型")
                           ,sGroupName+"/LaneType"
                           ,1
                           ,tr("车道类型")
                           ,tr("1")
                           ,tr("%1:\r%2;%3;")
                           .arg("当前车道的车道类型")
                           .arg("1--MTC入口\r2--MTC出口\r3--ETC入口\r4--ETC出口")
                           .arg("\r按照上面说明进行修改"));

    //item 02
    addNextLevelNodeToList(item
                           ,tr("启动调试")
                           ,sGroupName+"/Debug"
                           ,2
                           ,tr("启动调试")
                           ,tr("0")
                           ,tr("%1:\r%2;%3;")
                           .arg("当前收费软件是否启动调试模式")
                           .arg("0--不启动调试\r1--启动调试模式")
                           .arg("\r按照上面说明进行修改"));

    //item 03
    addNextLevelNodeToList(item
                           ,tr("参数文件路径")
                           ,sGroupName+"/ParamPath"
                           ,3
                           ,tr("启动调试")
                           ,tr("./cfg/")
                           ,tr("%1:\r%2;%3;")
                           .arg("参数文件存储的路径")
                           .arg("例如：./cfg/")
                           .arg("\r按照上面说明进行修改"));

    //item 1
    sGroupName = "StationInfo";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("收费站信息"));
    listCfgData.append(item);

    //item 11
    addNextLevelNodeToList(item
                           ,tr("收费站IP")
                           ,sGroupName+"/DataSrvIP"
                           ,5
                           ,tr("收费站IP")
                           ,tr("127.0.0.1")
                           ,tr("%1:\r%2;%3;")
                           .arg("收费中心的IP")
                           .arg("例如：127.0.0.1")
                           .arg("\r按照上面说明进行修改"));

    //item 12
    addNextLevelNodeToList(item
                           ,tr("收费站端口")
                           ,sGroupName+"/DataSrvPort"
                           ,6
                           ,tr("收费站端口")
                           ,tr("13334")
                           ,tr("%1:\r%2;%3;")
                           .arg("收费站通信端口")
                           .arg("例如:13334")
                           .arg("\r按照上面说明进行修改"));

    //item 13
    addNextLevelNodeToList(item
                           ,tr("监控服务IP")
                           ,sGroupName+"/MonSrvIP"
                           ,7
                           ,tr("监控服务IP")
                           ,tr("127.0.0.1")
                           ,tr("%1:\r%2;%3;")
                           .arg("监控服务IP")
                           .arg("例如:127.0.0.1")
                           .arg("\r按照上面说明进行修改"));

    //item 14
    addNextLevelNodeToList(item
                           ,tr("监控服务端口")
                           ,sGroupName+"/MonSrvPort"
                           ,8
                           ,tr("监控服务端口")
                           ,tr("7777")
                           ,tr("%1:\r%2;%3;")
                           .arg("监控服务通信端口")
                           .arg("例如:7777")
                           .arg("\r按照上面说明进行修改"));

    //item 15
    addNextLevelNodeToList(item
                           ,tr("图片保存天数")
                           ,sGroupName+"/PicSaveDay"
                           ,9
                           ,tr("图片保存天数")
                           ,tr("7")
                           ,tr("%1:\r%2;%3;")
                           .arg("抓拍图片保存本地的天数")
                           .arg("例如:7代表7天")
                           .arg("\r按照上面说明进行修改"));

    //item 16
    addNextLevelNodeToList(item
                           ,tr("XML存储天数")
                           ,sGroupName+"/XmlSaveDay"
                           ,10
                           ,tr("XML存储天数")
                           ,tr("7")
                           ,tr("%1:\r%2;%3;")
                           .arg("XML保存本地的天数")
                           .arg("例如:7代表7天")
                           .arg("\r按照上面说明进行修改"));

    //item 17
    addNextLevelNodeToList(item
                           ,tr("监控等待秒数")
                           ,sGroupName+"/MonWaitTime"
                           ,11
                           ,tr("监控等待秒数")
                           ,tr("20")
                           ,tr("%1:\r%2;%3;")
                           .arg("监控回控等待秒数")
                           .arg("例如:20代表20秒")
                           .arg("\r按照上面说明进行修改"));

    //item 18
    addNextLevelNodeToList(item
                           ,tr("图片存储路径")
                           ,sGroupName+"/PicPath"
                           ,12
                           ,tr("图片存储路径")
                           ,tr("/Program_Data/")
                           ,tr("%1:\r%2;%3;")
                           .arg("抓拍图像图片存储路径")
                           .arg("例如:/Program_Data/代表主目录下的Program_Data文件夹")
                           .arg("\r按照上面说明进行修改"));
    //item 19
    addNextLevelNodeToList(item
                           ,tr("sqlite数据保存时间")
                           ,sGroupName+"/SqlSaveDay"
                           ,13
                           ,tr("sqlite数据保存时间")
                           ,tr("7")
                           ,tr("%1:\r%2;%3;")
                           .arg("sqlite数据保存时间")
                           .arg("例如:7代表保存7天")
                           .arg("\r按照上面说明进行修改"));
    //item 19
    addNextLevelNodeToList(item
                           ,tr("最小运行空间")
                           ,sGroupName+"/DiskLimitSize"
                           ,14
                           ,tr("允许收费的最小运行空间(M)")
                           ,tr("10")
                           ,tr("%1:\r%2;%3;")
                           .arg("允许收费的最小运行空间(M)")
                           .arg("例如:10代表保存10M")
                           .arg("\r按照上面说明进行修改"));

    //item 2
    sGroupName = "LaneDBSrv";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("车道数据库服务"));
    listCfgData.append(item);

    //item 21
    addNextLevelNodeToList(item
                           ,tr("业务数据库的路径")
                           ,sGroupName+"/YWDataName"
                           ,16
                           ,tr("业务数据库的路径")
                           ,tr("./YeWu1.db")
                           ,tr("%1:\r%2;%3;")
                           .arg("业务数据库的存储的路径和名字")
                           .arg("例如:默认值代表当前文件夹下名字为YeWu1.db")
                           .arg("\r按照上面说明进行修改"));

    //item 3
    sGroupName = "Setup";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("其他设置"));
    listCfgData.append(item);

    //item 31
    addNextLevelNodeToList(item
                           ,tr("启用模拟键")
                           ,sGroupName+"/UseSimulate"
                           ,18
                           ,tr("启用模拟键")
                           ,tr("1")
                           ,tr("%1:\r%2;%3;")
                           .arg("是否启用后线圈过车模拟键")
                           .arg("例如:0--不启用，1--启用")
                           .arg("\r按照上面说明进行修改"));

    //item 32
    addNextLevelNodeToList(item
                           ,tr("超时停留时间")
                           ,sGroupName+"/StayOvertimeInterval"
                           ,19
                           ,tr("超时停留时间")
                           ,tr("40")
                           ,tr("%1:\r%2;%3;")
                           .arg("车辆过车后超时停留时间")
                           .arg("例如:默认值代表40秒")
                           .arg("\r按照上面说明进行修改"));

    //item 33
    addNextLevelNodeToList(item
                           ,tr("落杆延迟")
                           ,sGroupName+"/DownBarrierDelay"
                           ,20
                           ,tr("落杆延迟")
                           ,tr("1")
                           ,tr("%1:\r%2;%3;")
                           .arg("车辆过车后栏杆落下的延迟")
                           .arg("例如:默认值代表1秒")
                           .arg("\r按照上面说明进行修改"));

    //item 34
    addNextLevelNodeToList(item
                           ,tr("同车交易")
                           ,sGroupName+"/DownBarrierDelay"
                           ,21
                           ,tr("同车交易")
                           ,tr("5")
                           ,tr("%1:\r%2;%3;")
                           .arg("同一辆车在多长时间后允许再次交易")
                           .arg("例如:默认值代表5分钟")
                           .arg("\r按照上面说明进行修改"));

    //item 4
    sGroupName = "FD";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("费显-外设"));
    listCfgData.append(item);

    //item 41
    addNextLevelNodeToList(item
                           ,tr("费显-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,23
                           ,tr("费显-外设：驱动")
                           ,tr("./FD_TRD19D_LA.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("费显-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为FD_TRD19D_LA.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 42
    addNextLevelNodeToList(item
                           ,tr("费显-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,24
                           ,tr("费显-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("费显-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 43
    addNextLevelNodeToList(item
                           ,tr("费显-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,25
                           ,tr("费显-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("费显-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 5
    sGroupName = "IOCard";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("IO板-外设"));
    listCfgData.append(item);

    //item 51
    addNextLevelNodeToList(item
                           ,tr("IO板-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,27
                           ,tr("IO板-外设：驱动")
                           ,tr("./IOCard_PCL730.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("IO板-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为IOCard_PCL730.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 52
    addNextLevelNodeToList(item
                           ,tr("IO板-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,28
                           ,tr("IO板-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("IO板-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 53
    addNextLevelNodeToList(item
                           ,tr("IO板-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,29
                           ,tr("IO板-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("IO板-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 6
    sGroupName = "CardReader";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("卡读写器-外设"));
    listCfgData.append(item);

    //item 61
    addNextLevelNodeToList(item
                           ,tr("卡读写器-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,31
                           ,tr("卡读写器-外设：驱动")
                           ,tr("./JKM115_YN.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("卡读写器-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为JKM115_YN.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 62
    addNextLevelNodeToList(item
                           ,tr("卡读写器-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,32
                           ,tr("卡读写器-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("卡读写器-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 63
    addNextLevelNodeToList(item
                           ,tr("卡读写器-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,33
                           ,tr("卡读写器-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("卡读写器-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 7
    sGroupName = "VPR";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("车牌识别-外设"));
    listCfgData.append(item);

    //item 71
    addNextLevelNodeToList(item
                           ,tr("车牌识别-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,35
                           ,tr("车牌识别-外设：驱动")
                           ,tr("./VPR_LP_V.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("车牌识别-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为VPR_LP_V.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 72
    addNextLevelNodeToList(item
                           ,tr("车牌识别-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,36
                           ,tr("车牌识别-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("车牌识别-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 73
    addNextLevelNodeToList(item
                           ,tr("车牌识别-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,37
                           ,tr("车牌识别-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("车牌识别-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 8
    sGroupName = "Printer";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("打印机-外设"));
    listCfgData.append(item);

    //item 81
    addNextLevelNodeToList(item
                           ,tr("打印机-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,39
                           ,tr("打印机-外设：驱动")
                           ,tr("./NotePrinter.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("打印机-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为NotePrinter.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 82
    addNextLevelNodeToList(item
                           ,tr("打印机-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,40
                           ,tr("打印机-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("打印机-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 83
    addNextLevelNodeToList(item
                           ,tr("打印机-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,41
                           ,tr("打印机-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("打印机-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 9
    sGroupName = "VDM";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("字符叠加-外设"));
    listCfgData.append(item);

    //item 91
    addNextLevelNodeToList(item
                           ,tr("字符叠加-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,43
                           ,tr("字符叠加-外设：驱动")
                           ,tr("./NotePrinter.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("字符叠加-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为NotePrinter.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 92
    addNextLevelNodeToList(item
                           ,tr("字符叠加-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,44
                           ,tr("字符叠加-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("字符叠加-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 93
    addNextLevelNodeToList(item
                           ,tr("字符叠加-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,45
                           ,tr("字符叠加-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("字符叠加-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 10
    sGroupName = "RSU";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("天线-外设"));
    listCfgData.append(item);

    //item 101
    addNextLevelNodeToList(item
                           ,tr("天线-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,47
                           ,tr("天线-外设：驱动")
                           ,tr("./Test.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为Test.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 102
    addNextLevelNodeToList(item
                           ,tr("天线-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,48
                           ,tr("天线-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 103
    addNextLevelNodeToList(item
                           ,tr("天线-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,49
                           ,tr("天线-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 104
    addNextLevelNodeToList(item
                           ,tr("天线功率")
                           ,sGroupName+"/power"
                           ,50
                           ,tr("天线功率")
                           ,tr("2")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线的工作功率")
                           .arg("例如:默认值代表功率为2db")
                           .arg("\r按照上面说明进行修改"));

    //item 105
    addNextLevelNodeToList(item
                           ,tr("OBU重读时间")
                           ,sGroupName+"/OBURetryTime"
                           ,51
                           ,tr("OBU重读时间")
                           ,tr("10")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线OBU的重读时间")
                           .arg("例如:默认值代表重读时间为10")
                           .arg("\r按照上面说明进行修改"));

    //item 106
    addNextLevelNodeToList(item
                           ,tr("允许拆卸")
                           ,sGroupName+"/AllowDisassembly"
                           ,52
                           ,tr("允许拆卸")
                           ,tr("0")
                           ,tr("%1:\r%2;%3;")
                           .arg("天线OBU的是否允许拆卸")
                           .arg("例如:0--可以拆卸,1--不可以拆卸")
                           .arg("\r按照上面说明进行修改"));

    //item 107
    addNextLevelNodeToList(item
                           ,tr("允许车牌不绑定")
                           ,sGroupName+"/AllowNotBinding"
                           ,53
                           ,tr("允许车牌不绑定")
                           ,tr("0")
                           ,tr("%1:\r%2;%3;")
                           .arg("是否允许车牌不绑定")
                           .arg("例如:0--允许,1--不允许")
                           .arg("\r按照上面说明进行修改"));

    //item 11
    sGroupName = "WeightDev";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("计重-外设"));
    listCfgData.append(item);

    //item 111
    addNextLevelNodeToList(item
                           ,tr("计重-外设：驱动")
                           ,sGroupName+"/Driver"
                           ,55
                           ,tr("计重-外设：驱动")
                           ,tr("./Test.dll")
                           ,tr("%1:\r%2;%3;")
                           .arg("计重-外设的驱动库存放路径")
                           .arg("例如:默认值代表当前目录下名字为Test.dll")
                           .arg("\r按照上面说明进行修改"));

    //item 112
    addNextLevelNodeToList(item
                           ,tr("计重-外设：参数1")
                           ,sGroupName+"/ConnStr1"
                           ,56
                           ,tr("计重-外设：参数1")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("计重-外设的参数1")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));

    //item 113
    addNextLevelNodeToList(item
                           ,tr("计重-外设：参数2")
                           ,sGroupName+"/ConnStr2"
                           ,57
                           ,tr("计重-外设：参数2")
                           ,tr("123")
                           ,tr("%1:\r%2;%3;")
                           .arg("计重-外设的参数2")
                           .arg("例如:按照设备不同代表意义不同")
                           .arg("\r按照上面说明进行修改"));
    //item 12
    sGroupName = "Other";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("其他"));
    listCfgData.append(item);

    //item 121
    addNextLevelNodeToList(item
                           ,tr("SD卡路径")
                           ,sGroupName+"/SDName"
                           ,59
                           ,tr("SD卡路径")
                           ,tr("I:/")
                           ,tr("%1:\r%2;%3;")
                           .arg("SD卡路径")
                           .arg("例如:默认值表示存储在I盘根目录下")
                           .arg("\r按照上面说明进行修改"));
    //item 13
    sGroupName = "BarControl";
    item=new QTreeWidgetItem(widget);
    item->setText(0,tr("栏杆控制"));
    listCfgData.append(item);

    //item 131
    addNextLevelNodeToList(item
                           ,tr("防砸车，落杆延迟")
                           ,sGroupName+"/MSecond"
                           ,61
                           ,tr("防砸车，落杆延迟")
                           ,tr("1000")
                           ,tr("%1:\r%2;%3;")
                           .arg("防砸车，落杆延迟")
                           .arg("例如:默认值表示栏杆落下延迟1000毫秒")
                           .arg("\r按照上面说明进行修改"));
}

QStringList MaintainCFG::setColumnName(QString name)
{
    //设置栏位数量和栏位名称
    m_ColumnTitle.append(name);
    return m_ColumnTitle;
}

QList<QTreeWidgetItem *> MaintainCFG::getTopNode()
{
    return listCfgData;
}

void MaintainCFG::addNextLevelNodeToList(QTreeWidgetItem *parent
                                         ,QString childName
                                         ,QString cfgValuePath
                                         , quint8 code
                                         , QString name
                                         , QString defaultValue
                                         , QString introduce)
{
    QTreeWidgetItem *item_child=new QTreeWidgetItem(parent);
    item_child->setText(0,childName);
    listCfgData.append(item_child);
    QSettings m_Settngs(getCfgPath()+"/"+getLaneCfgName(),QSettings::IniFormat);
    cfgNode *node=new cfgNode;
    node->code=code;
    node->cfg_currentValue=m_Settngs.value(cfgValuePath).toString();
    node->cfg_defaultValue=defaultValue;
    node->cfg_ModifyValue.clear();
    node->cfg_name=name;
    node->cfg_introduce=introduce;
    node->cfg_Path=cfgValuePath;
    listCfgNode.append(node);
    mapHistory.insert(node->code,node->cfg_currentValue);
}

void MaintainCFG::setUpdateFilePath(QString path)
{
    m_UpdateFilePath=path;
}

void MaintainCFG::setUpdateFileName(QString name)
{
    m_UpdateFileName=name;
}

QString MaintainCFG::getUpdateFilePath()
{
    return m_UpdateFilePath;
}

QString MaintainCFG::getUpdateFileName()
{
    return m_UpdateFileName;
}

QString MaintainCFG::getOldFilePath()
{
    return m_OldFilePath;
}

void MaintainCFG::setOldFilePath(QString path)
{
    m_OldFilePath=path;
}

void MaintainCFG::setPwdMD5(QString pwd)
{
    m_pwd=pwd;
}

QString MaintainCFG::getPwdMd5()
{
    return m_pwd;
}

bool MaintainCFG::isHaveModify()
{
    bool flag=false;
    for(int index=0;listCfgNode.size()!=index;++index)
    {
        if(!listCfgNode.at(index)->cfg_ModifyValue.isEmpty())
        {
            flag=true;
            break;
        }
    }
    return flag;
}

void MaintainCFG::WriteCfg(QString path, QString value)
{
    QSettings m_Settings(getCfgPath()+"/"+getLaneCfgName(),QSettings::IniFormat);
    m_Settings.setValue(path,value);
}

void MaintainCFG::setLaneCfg(QString LaneCfgName)
{
    m_LaneCfgName=LaneCfgName;
}

QString MaintainCFG::getLaneCfgName()
{
    return m_LaneCfgName;
}

void MaintainCFG::setSoftRole(int role)
{
    m_SoftRole = role;
}

int MaintainCFG::getSoftRole()
{
    return m_SoftRole;
}

void MaintainCFG::setDBName(QString name)
{
    m_DBName = name;
}

QString MaintainCFG::getDBName()
{
    return m_DBName;
}
