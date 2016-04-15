#ifndef TESTFD_H
#define TESTFD_H

#include <QWidget>
#include "BaseInterface/devbaseinterface.h"
#include "FareDisplayArm/faredisplayarm.h"
namespace Ui {
    class TestFD;
}

class TestFD : public QWidget
{
    Q_OBJECT

public:
    explicit TestFD(QWidget *parent = 0);
    ~TestFD();
private slots:
    void on_pushButton_openDev_clicked();

    void on_pushButton_closeDev_clicked();

    void on_pushButton_vehClass_clicked();

    void on_pushButton_Fare_clicked();

    void on_pushButton_Before_clicked();

    void on_pushButton_After_clicked();

    void on_pushButton_Weight_clicked();

    void on_pushButton_Text_clicked();
    void on_pushButton_EnStation_clicked();

    void on_pushButton_ClearAll_clicked();

    void on_pushButton_StarAlarm_clicked();

    void on_pushButton_EndAlarm_clicked();

private:
    Ui::TestFD *ui;
    DevBaseInterface* tmpDev;
    FareDisplayArm* m_fd;
};

#endif // TESTFD_H
