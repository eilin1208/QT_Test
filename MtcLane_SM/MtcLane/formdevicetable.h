#ifndef FORMDEVICETABLE_H
#define FORMDEVICETABLE_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "devicefactory.h"
namespace Ui {
class FormDeviceTable;
}

class FormDeviceTable : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormDeviceTable(QWidget *parent = 0, QLayout* layout = 0);
    ~FormDeviceTable();
public slots:
    void showDevStatus(int status);
protected:
    void showEvent(QShowEvent *);
private:
    void showDevInfo(const QString& dev, int status);
private:
    Ui::FormDeviceTable *ui;
};

#endif // FORMDEVICETABLE_H
