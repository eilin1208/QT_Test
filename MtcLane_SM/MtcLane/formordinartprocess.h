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
#include "formpapercard.h"
#include "formtablewidget.h"
#include "formpasscard.h"
#include "formbadcard.h"
#include "formcalchange.h"
#include "formmessage.h"
#include "formvehqueprocess.h"
#include "formpasswordverify.h"
#include "formvehquebegin.h"
#include "formamendweight.h"
#include "forminformid.h"
#include "formmodifyweight.h"

namespace Ui {
class FormOrdinartProcess;
}

class FormOrdinaryProcess : public MtcOpWidget
{
    Q_OBJECT
    Q_PROPERTY(bool entryInfo READ entryInfo WRITE setShowEntry)
public:
    explicit FormOrdinaryProcess(QWidget *parent = 0,QLayout *layout = 0);
    virtual ~FormOrdinaryProcess();
    void buildSubStateMachine(QState *parent, QState* next = NULL);

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

protected:

public slots:
    //    //接收到有卡信号
    //    void ReceiveCardsignal();
    //    //接收线圈变化信号
    //    void ReceiveRingsignal(int state);
public:

private:

    //创建子窗口
    void buildChildrenWidget();
private:
    ForminformId *m_informTK;
    FormEntryPlate* m_pPlate;
    FormEntryVehInfo* m_pVehInfo;
    FormPaperCard* m_pPaperCard;
    FormTableWidget* m_pTable;
    QVBoxLayout* m_layout;
    FormPassCard* m_pPassCard;
    FormBadCard* m_pBadCard;
    FormCalChange* m_pCalChange;
   // Formaccredit * m_pAccedit;
    FormModifyWeight *m_pModifyWeight;
    //车队
    FormMessage *m_pMessage;
    //FormVehqueProcess * m_pAuth;
    FormPasswordVerify *m_pAuthByCap;
    FormVehQueBegin *m_QueBegin;
    Ui::FormOrdinartProcess *ui;

};

FormOrdinaryProcess* getOrdinaryWidget();

#endif // FORMORDINARTPROCESS_H
