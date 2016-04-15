#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "VPRArm/vprarm.h"
#include "BaseInterface/devbaseinterface.h"
#include <QPalette>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void GetPlateColor(int nColor);
protected:
    void customEvent(QEvent *event);

private slots:
    void on_pushButton_OpenDev_clicked();

    void on_pushButton_CloseDev_clicked();

    void on_pushButton_Capture_clicked();

private:
    Ui::Widget *ui;
    DevBaseInterface* tmpDev;
    VPRArm *m_qVpr;
    QPalette m_palette;
};

#endif // WIDGET_H
