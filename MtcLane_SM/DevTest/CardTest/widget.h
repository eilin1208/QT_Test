#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "CardReaderArm/qcardreader.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void getCardTollInfo(TCardTollInfo CardTollInfo);
private slots:
    void on_pb_init_clicked();

    void on_pb_close_clicked();

    void on_pb_readcard_clicked();

    void on_writecard_clicked();

private:
    Ui::Widget *ui;
    DevBaseInterface* tmpDev;
    QCardReader* reader;
    QString tmpcardid;
    QString m_str;
};

#endif // WIDGET_H
