#ifndef WIDGET_H
#define WIDGET_H
#include "portcomm.h"
#include <QWidget>
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
    void on_push_auto_clicked();

    void on_push_one_clicked();

    void on_push_init_clicked();

    void on_push_close_clicked();

    void on_push_closeauto_clicked();

    void ShowString();

    void ReadySend();

    void SendInit();

    void SendAuto();

    void on_push_clear_clicked();

private:
    Ui::Widget *ui;
    QTimer *timer;
    QTimer *inittimer;
    BStatus m_status;
};

#endif // WIDGET_H
