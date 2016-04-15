#ifndef FORMINFORMID_H
#define FORMINFORMID_H

#include <QWidget>
#include <QLineEdit>
#include"MtcKey/MtcOpWidget.h"
#include"MtcKey/MtcKeySignalTransition.h"
#include"QAbstractState"
namespace Ui {
class ForminformId;
}

class ForminformId : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit ForminformId(QWidget *parent = 0,QLayout *layout=0);
    virtual ~ForminformId();
public:
    bool IsEnterInvFinished(QString & sError);
    void clearData();
    qint64 getNewInvoiceNo();
    void setInfoTitle1();
    void setInfoTitle2();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *event);
private:
    void handleNumberKey(MtcKeyPressedEvent* mtcKeyEvent);
    void handleEnterKey(MtcKeyPressedEvent* mtcKeyEvent);
    void handleEraserKey();
    void handleEscKey(MtcKeyPressedEvent* mtcKeyEvent);
private:
    Ui::ForminformId *ui;
private:
    QString m_sLastInvNo;
    QLineEdit *m_pCurLineEdit;
    int m_ticketLength;
};

ForminformId* getInformWidget(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMINFORMID_H
