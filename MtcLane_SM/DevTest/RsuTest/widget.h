#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "RSUArm/rsudatatype.h"

namespace Ui {
    class Widget;
}

class rsuctrl;
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_startRsu_clicked();

    void on_stopRsu_clicked();

    void onStatusChangedSlot(int);
    void getFrameDataSlot(QByteArray frameData);
    void getSendingFrameDataSlot(QByteArray frameData);

    void getB0InfoSlot();
    void getB2InfoSlot(QVariant ,QVariant );
    void getB3InfoSlot(QVariant ,QVariant );
    void getB4InfoSlot(QVariant ,QVariant );
    void getB5InfoSlot(QVariant ,QVariant );
    void getB6InfoSlot(QVariant ,QVariant );

    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    //rsuctrl *rsuCtrl;
};

#endif // WIDGET_H
