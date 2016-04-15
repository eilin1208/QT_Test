#ifndef FORMAUTOVEHINFO_H
#define FORMAUTOVEHINFO_H
#include <QVector>
#include <QThread>
#include "MtcKey/MtcOpWidget.h"
#include "devicefactory.h"
#include "formlog.h"
namespace Ui {
class FormAutoVehInfo;
}

class FormAutoVehInfo : public MtcOpWidget
{
    Q_OBJECT
    
signals:
    /*!
     \brief
    处理pro卡成功
    */
    void handleProCardSuccess();
public:
    explicit FormAutoVehInfo(QWidget *parent = 0, QLayout* layout = 0);
    ~FormAutoVehInfo();
public:
    void clearData();
    void login(bool isLogin);

private slots:
    /*!
     \brief
    处理卡机用户按键
     \param nPos
     0代表上机位，1代表下机位
    */
    void handleButtonPush(int pos);

    /*!
     \brief
    处理卡交易
     \param cardId
     卡ID
     \param cardType
     卡类型
    */
    void handleCard(quint32 cardId, int cardType);
    void getVpr(int vprIndex);

    void getDIStatus(quint32 input, quint8 status);
private:
    int getCardReaderId(QObject* sender);

    //显示pro车辆信息
    void showVehProInfo();
private:
    Ui::FormAutoVehInfo *ui;
    //日志窗口
    FormLog* m_logWidget;
    int m_cardMgrPos;
    //是否是第一次按键
    bool m_isFirstPush;
};

#endif // FORMAUTOVEHINFO_H
