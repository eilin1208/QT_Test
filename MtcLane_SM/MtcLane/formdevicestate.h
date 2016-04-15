#ifndef FORMDEVICESTATE_H
#define FORMDEVICESTATE_H

#include <QWidget>
#include "devicefactory.h"

namespace Ui {
    class FormDeviceState;
}

class FormDeviceState : public QWidget
{
    Q_OBJECT

public:
    explicit FormDeviceState(QWidget *parent = 0);
    virtual ~FormDeviceState();
    void Init();
public slots:
    void IoOuputChanged(int nDevice,int nStatus);
    void IoInputChanged(quint32 dev,quint8 nStatus);
    void printerStatusChanged(int status);
    void deviceStatusChanged(int status);
    void allowCarPass(int nNum);
    void LockBar(bool bLock);
private:
    Ui::FormDeviceState *ui;
};
#endif // FORMDEVICESTATE_H
