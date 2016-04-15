#ifndef TESTVPR_H
#define TESTVPR_H

#include <QWidget>
#include "VPRArm/vprarm.h"
#include "BaseInterface/devbaseinterface.h"
#include <QPalette>
namespace Ui {
    class TestVpr;
}

class TestVpr : public QWidget
{
    Q_OBJECT

public:
    explicit TestVpr(QWidget *parent = 0);
    ~TestVpr();

    void GetPlateColor(int nColor);
protected:
    void customEvent(QEvent *event);

private slots:
    void on_pushButton_OpenDev_clicked();

    void on_pushButton_CloseDev_clicked();

    void on_pushButton_Capture_clicked();
private:
    Ui::TestVpr *ui;
    DevBaseInterface* tmpDev;
    VPRArm *m_qVpr;
    QPalette m_palette;
};

#endif // TESTVPR_H
