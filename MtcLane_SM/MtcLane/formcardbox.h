#ifndef FORMCARDBOX_H
#define FORMCARDBOX_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "CommonModule/CardFileDef.h"
namespace Ui {
class FormCardBox;
}

class FormCardBox : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormCardBox(QWidget *parent = 0, QLayout* layout = NULL);
    virtual ~FormCardBox();
    bool CardBoxRegister(bool bCardBoxUp);
public slots:
    void DetectCard(quint32 nDetectId, int nCardType);
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    //装盒
    bool LoadCardBox(quint32 nCardBoxNo, quint32 nFlagCardNo, quint16 nCount);
    //卸下卡盒
    bool UnLoadCardBox(int nOperType);
    //卡盒换上处理
    bool CardBoxUp(TBoxCardBasicInfo& basicInfo, TBoxCardAppInfo& appInfo);
    //卡盒换下处理
    bool CardBoxDown(TBoxCardBasicInfo& basicInfo, TBoxCardAppInfo& appInfo);
    //换卡盒卡处理
    bool HandBoxCard(TBoxCardBasicInfo& basicInfo, TBoxCardAppInfo& appInfo);
private:
    Ui::FormCardBox *ui;
    //卡盒换上换下操作
    bool m_bCardBoxUp;
};

FormCardBox *getCardBox(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMCARDBOX_H
