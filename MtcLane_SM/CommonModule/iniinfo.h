#ifndef INIINFO_H
#define INIINFO_H

#include <QObject>
#include <QSettings>
/*!
 \brief
    用于读取Lane.ini文件除设备驱动信息外的软件配置信息
*/
class IniInfo : public QObject
{
    Q_OBJECT
public:

    /*!
     \brief
     构造函数
     \param parent
    */
    explicit IniInfo(QObject *parent = 0);
    /*!
     \brief
     获取车道类型
     \return int
     车道类型编码
    */
    int getLaneType();
    /*!
     \brief
     获取车道ID
     \return int
     车道ID号
    */
    int getLaneId();

    /*!
     \brief
     获取参数文件存储路径
     \return QString
     返回值为绝对路径
    */
    QString getParamPath();
    /*!
     \brief
     获取数据服务器IP
     \return QString
    */
    QString getDataSrvIp();
    /*!
     \brief
     获取数据服务器端口号
     \return quint32
    */
    quint32 getDataSrvPort();
    /*!
     \brief
     获取监控服务器IP
     \return QString
    */
    QString getMonSrvIp();
    /*!
     \brief
     获取监控服务器端口号
     \return quint32
    */
    quint32 getMonSrvPort();
    /*!
     \brief
     获取图片保存时间
     \return int
     返回值以天为单位
    */
    int getPicSaveDays();
    /*!
     \brief
     获取XML保存时间

     \return int
    */
    int getXmlSaveDays();
    /*!
     \brief
     获取业务数据库数据保存时间
     \return int
     返回值以天为单位
    */
    int getSqlSaveDays();
    /*!
     \brief
     获取软件运行所需硬盘最小限度
     \return int
     返回值以M为单位
    */
    int getDiskLimitSize();
    /*!
     \brief
     获取本地sqlite业务数据库名称
     \return QString
    */
    QString getYWDataName();
    /*!
     \brief
     获取XML存放路径
     \return QString
    */
    QString getXMLPath();
    /*!
     \brief
     获取外接移动存储设备名称
     \return QString
    */
    QString getSDName();
    /*!
     \brief
     获取是否允许使用模拟键
     \return int
     1允许，0禁止
    */
    int getUseSimulate();
    /*!
     \brief
     获取是否允许前线圈无车发卡
     \return int
     1允许，0禁止
    */
    int getUseFrontLoop();
    /*!
     \brief
     是否使用车牌识别
     \return int
     1允许，0禁止
    */
    int getUsePlate();
    /*!
     \brief
     是否使用计重收费
     \return int
     1允许，0禁止
    */
    int getUseWeight();
    /*!
     \brief
     获取防砸杆时间
     \return int
     1允许，0禁止
    */
    int getBarControl();

signals:
    
public slots:
protected:
    /*!
     \brief

     \param sKey
     \return QVariant
    */
    QVariant getValue(const QString& sKey);
};

#endif // INIINFO_H
