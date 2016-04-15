#ifndef FORMVEHQUEPROCESS_H
#define FORMVEHQUEPROCESS_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
    class FormVehqueProcess;
}

class FormVehqueProcess : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit FormVehqueProcess(QWidget *parent = 0,QLayout *layout = 0);
    virtual ~FormVehqueProcess();
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void resetCount();
public slots:
     void setCount();
     void startCount();
signals:
     void begin();
     void monitor();
private:
    Ui::FormVehqueProcess *ui;
    int count;
    QTimer *timer;
};

#endif // FORMVEHQUEPROCESS_H
