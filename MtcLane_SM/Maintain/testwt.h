#ifndef TESTWT_H
#define TESTWT_H

#include <QWidget>
#include <QTableWidgetItem>
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "WtArm/wtdevunit.h"
namespace Ui {
    class TestWt;
}

class TestWt : public QWidget
{
    Q_OBJECT

public:
    explicit TestWt(QWidget *parent = 0);
    ~TestWt();

    void changeData();

public slots:
    void WtDataChanged(int nEvent, int nId);

    void GetDeleteRow(QTableWidgetItem * item);

private slots:
    void on_pushButton_init_clicked();

    void on_pushButton_close_clicked();
    void on_pushButton_clearWtInfo_clicked();

    void on_pushButton_clicked();

private:
    Ui::TestWt *ui;
    DevBaseInterface* tmpDev;
    TWtDev* wtDev;
    int DeleteRow;
};

#endif // TESTWT_H
