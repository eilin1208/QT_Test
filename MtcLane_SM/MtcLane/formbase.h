#ifndef FORMORDINARTPROCESS_H
#define FORMORDINARTPROCESS_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "MtcKey/MtcKeySignalTransition.h"
#include <QStringList>
#include "LocalCfgDb/LocalCfgDb.h"
#include <QState>
#include <QVBoxLayout>
#include "formentryplate.h"
#include "formentryvehinfo.h"

#include "formtablewidget.h"

#include "formmessage.h"
#include "formvehquebegin.h"
#include "statemachinetransition.h"
namespace Ui {
class FormOrdinartProcess;
}

class FormBase : public MtcOpWidget
{
    Q_OBJECT
    Q_PROPERTY(bool entryInfo READ entryInfo WRITE setShowEntry)
public slots:

public:
    explicit FormBase(QWidget *parent = 0,QLayout *layout = 0);
    virtual ~FormBase();

    void clearInfo();
    void setEntryInfo(const QDateTime& entryTime, const QString& sPlate, const QString& sStation, const QString& sCardNo,
                      const QString& sWorkNo, const QString& driveTime, const QString& sFlag);
    bool entryInfo()
    {
        return true;
    }
    void setShowEntry(bool bShow)
    {
        if(bShow)
        {
            showEntryInfo();
        }
        else
        {
            hideEntryInfo();
        }
    }
    //    QString m_inPut;
    //隐藏出口上部分内容
    void hideEntryInfo();
    //显示出口上部分内容
    void showEntryInfo();
public:
    //创建子状态机
    void initStateMachine(QState* parent, QState* next = NULL);
protected:
    //创建子窗口
    virtual void createChildrenWidget();
    //创建子状态机
    virtual void buildSubStateMachine();
    //实例化状态
    virtual void createStates();
protected:
    //父状态
    QState* m_parentState;
    //退出状态
    QState* m_nextState;

    QWidget* m_widget;
    QVBoxLayout* m_layout;

protected:
    //公共状态
    QState* m_initState;
    //显示功能状态
    QState* m_funcState;
    //设置状态
    QState* m_setupState;
    //完成处理状态
    QState* m_finState;
    //车队状态
    QState* m_motorState;
    //闯关状态
    QState* m_rushState;
    //其他免征车辆
    QState* m_otherState;

protected:
    //车牌输入
    FormEntryPlate* m_pPlate;
    //车辆信息输入
    FormEntryVehInfo* m_pVehInfo;
    //列表框
    FormTableWidget* m_pTable;
    //提示信息
    FormMessage *m_pMessage;
    //车队
    FormVehQueBegin *m_pMotor;
private:
    Ui::FormOrdinartProcess *ui;
};

FormBase* getOrdinaryWidget();

#endif // FORMORDINARTPROCESS_H
