#ifndef FORMWTINFO_H
#define FORMWTINFO_H
#include <QWidget>
#include <QTableWidget>
#include "WtArm/wtdatadefineunit.h"
#include "WtArm/wtdatamgrunit.h"
namespace Ui {
class FormWtInfo;
}

class FormWtInfo : public QWidget
{
    Q_OBJECT
public slots:
    void WtDataChanged(int nEvent, int nId);

public:
    explicit FormWtInfo(QWidget *parent = 0);
    virtual ~FormWtInfo();
    void changeData();
    void showHeadData();
private:
    Ui::FormWtInfo *ui;
};
#endif // FORMWTINFO_H
