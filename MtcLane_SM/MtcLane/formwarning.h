#ifndef FORMWARNING_H
#define FORMWARNING_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
    class FormWarning;
}

class FormWarning : public QWidget
{
    Q_OBJECT
public:
    enum WARNINGTYPE
    {
        WARNING,
        MESSAGE
    };
public:
    explicit FormWarning(QWidget *parent = 0);
    virtual ~FormWarning();
    void setWarning(const QString& sMsg,bool bOpenTimer = true, WARNINGTYPE type = MESSAGE);
public:
    void startShowtimer();
public slots:
    void clearText();
protected:
    void timerEvent(QTimerEvent *event);
private:
    Ui::FormWarning *ui;
    int m_nTimerId;
};

#endif // FORMWARNING_H
