#ifndef FORMMODIFYWEIGHT_H
#define FORMMODIFYWEIGHT_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormModifyWeight;
}
enum TEditStatus
{
    esInit,         //初始化
    esWeight,       //总重
    esAxisNo,       //轴序号
    esAxisCount     //轴数
};

class FormModifyWeight : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormModifyWeight(QWidget *parent = 0, QLayout* layout = NULL);
    ~FormModifyWeight();
public slots:
    void WtDataChanged(int nEvent, int nId);
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *);
private:
    void addNew();
    void deleteOne();
    void modifyOne();
    void split();
    void combine();
    void clearAll();
    void setEditStatus(TEditStatus status);
    void showInputInfo(const QString& sInfo);
    void chooseFunc(MtcKeyPressedEvent* mtcKeyEvent);
    void inputWeight(MtcKeyPressedEvent* mtcKeyEvent);
    void inputAxisNo(MtcKeyPressedEvent* mtcKeyEvent);
    void inputAxisCount(MtcKeyPressedEvent* mtcKeyEvent);
    double getNumber(QString sNum);
    void changeData();
private:
    Ui::FormModifyWeight *ui;
    int m_nStatus;
    double m_dTotalWieght;
    int m_nAxisCount;
    int m_nAxisNo;
    int m_bModify;
    int m_nEditStatus;
};

FormModifyWeight* getModifyWeight(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMMODIFYWEIGHT_H
