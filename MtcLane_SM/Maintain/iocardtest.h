#ifndef IOCARDTEST_H
#define IOCARDTEST_H
#include "BaseInterface/devbaseinterface.h"
#include "IoBoardArm/devioboard.h"
#include <QWidget>
namespace Ui {
    class IoCardTest;
}

class IoCardTest : public QWidget
{
    Q_OBJECT

public:
    explicit IoCardTest(QWidget *parent = 0);
    ~IoCardTest();

public slots:
    void IOInputChanged(quint32 dev, quint8 nStatus);

private slots:
    void on_pBtn_Init_clicked();

    void on_pushButton_CloseDev_clicked();

    void on_pushButton_CanopyGreen_clicked();

    void on_pushButton_PassGreen_clicked();

    void on_pushButton_CanopyRed_clicked();

    void on_pushButton_PassRed_clicked();

    void on_pushButton_BarriarUp_clicked();

    void on_pushButton_BarriarDown_clicked();

    void on_pushButton_StartAlarm_clicked();

    void on_pushButton_EndAlarm_clicked();

private:
    Ui::IoCardTest *ui;
    DevBaseInterface* tmpDev;
    IOBoardArm *m_ioBoard;
    bool m_Inited;
};
#endif // IOCARDTEST_H
