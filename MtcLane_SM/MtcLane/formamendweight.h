#ifndef FORMAMENDWEIGHT_H
#define FORMAMENDWEIGHT_H

#include <QWidget>
#include <QTableWidget>
#include <MtcKey/MtcOpWidget.h>
#include <QKeyEvent>
#include "formeditwt.h"
#include <QMutexLocker>
#include "formmessage.h"
namespace Ui {
class Formamendweight;
}

class Formamendweight : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit Formamendweight(QWidget *parent = 0,QLayout *layout=0);
    virtual ~Formamendweight();
public:
    void ShowEditWt();
    void CalcLeftTop();
    void on_pushButton_3_clicked();
    void setindex(int dex);
   int Getindex();
   void setflagindex(int index);
   int Getflagindex();
   //计算超限率
   int OverWeightRate(int nTotalWeight,int nAxisNum);
   //返回执行的行数liubo
   int m_nCurrentRow;
   void setCurrentRow(int m_row);
   int getCurrentRow();
protected:
    void handleUpAndDown(MtcKeyPressedEvent* mtcKeyEvent);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();


    void on_pushButton_6_clicked();

    void DataEntered(int nWeight, int nAxls);
    void DataAmend(int nWeight,int nAxls);
    void on_pushButton_4_clicked();

private:
    void showHHeadNum();

private:
    Ui::Formamendweight *ui;
    QTableWidget *table;
    FormEditWt m_editWt;
    int m_nEditLeft;
    int m_nEditTop;
    int m_wight;
    int m_Axin;
    int index;
    int flagindex;
};
Formamendweight * getAmendweight(QWidget *widget = NULL,QLayout *layout =NULL);
#endif // FORMAMENDWEIGHT_H
