#ifndef FORMPAPERCARD_H
#define FORMPAPERCARD_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
class FormPaperCard;
}

class FormPaperCard : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormPaperCard(QWidget *parent = 0, QLayout* layout = 0);
    virtual ~FormPaperCard();
    QString getEnterNo();
signals:
    void entryStationRight();
    void entryStationWrong();
protected:
    //这里实现按键输入
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    //验证用户输入的编号是否有效，
    //如果有效则发送entryStationRight信号，
    //如果无效则发送entryStationWrong信号
    //用户输入的编号要调用QOrgCodeParamFile里面的queryOrgCodeValue函数进行验证
    void verifyInputNum();
    void showEvent(QShowEvent *event);
private:
    Ui::FormPaperCard *ui;
};

#endif // FORMPAPERCARD_H
