#ifndef FORMMTCTitle_H
#define FORMMTCTitle_H

#include <QWidget>
#include <QDateTime>
#include "ui_formmtctitle.h"
namespace Ui {
    class FormMTCTitle;
}

class FormMTCTitle : public QWidget
{
    Q_OBJECT
public:
    explicit FormMTCTitle(QWidget *parent = 0);
    virtual ~FormMTCTitle();
    void setOperName(const QString& sName);
    void setInvoice(qint64 nInvBeginNo, quint32 nInvCount);
    void setStaName(const QString& sName);
    void setStaType(int nLaneType, int nLaneCode);
    void setWorkState(int nState);
    void setShift(const QString& sShift, const QDateTime& iDataTime);
    void clearInfo();
    void setCardBoxNum(const QString& num);
    void setCardNum(quint32 num);
protected:
    virtual void showEvent(QShowEvent *event);
private:
    Ui::FormMTCTitle *ui;
};

#endif // FORMMTCTitle_H
