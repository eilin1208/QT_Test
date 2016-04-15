#ifndef FORMWTDETAIL_H
#define FORMWTDETAIL_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormWtDetail;
}

class FormWtDetail : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormWtDetail(QWidget *parent = 0, QLayout* layout = 0);
    ~FormWtDetail();
    void changeData();
public slots:
    void WtDataChanged(int nEvent, int nId);
    void WtStatusChanged(int nStatus);
protected:
    void showEvent(QShowEvent *);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private slots:
    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    Ui::FormWtDetail *ui;
};

FormWtDetail* getWtDetail(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMWTDETAIL_H
