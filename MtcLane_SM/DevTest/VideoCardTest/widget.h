#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "BaseInterface/devbaseinterface.h"
#include "VideoCardArm/videocardarm.h"
#include <QTimer>
namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_capture_clicked();

    void on_pushButton_init_clicked();

    void on_pushButton_close_clicked();

    void AutoCapture(bool clicked);

private:
    Ui::Widget *ui;
    DevBaseInterface* tmpDev;
    VideoCard_ARM* videocard;
    QString m_filename;
    QTimer* m_timer;
};

#endif // WIDGET_H
