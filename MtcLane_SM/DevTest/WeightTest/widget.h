#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "WtArm/wtdevunit.h"
namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    ~Widget();

    void changeData();

public slots:
    void WtDataChanged(int nEvent, int nId);

    void GetDeleteRow(QTableWidgetItem * item);
private slots:
    void on_pushButton_init_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_ClearOne_clicked();

    void on_pushButton_ClearAll_clicked();

    void on_pushButton_MergeWt_clicked();

    void on_pushButton_splitWt_clicked();

private:
    Ui::Widget *ui;
    DevBaseInterface* tmpDev;
    TWtDev* wtDev;
    int DeleteRow;
};

#endif // WIDGET_H
