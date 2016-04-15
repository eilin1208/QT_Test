#ifndef FORMENTRYREVERSE_H
#define FORMENTRYREVERSE_H

#include <QWidget>
#include "entrycardinfoprocessor.h"
#include "lanectrl.h"
#include "formmainwindow.h"
#include "formdevicestate.h"
#include "laneinfo.h"
namespace Ui {
    class FormEntryReverse;
}

class FormEntryReverse : public MtcOpWidget
{
    Q_OBJECT
     Q_PROPERTY(QString message READ getMessage WRITE setMessage)
public slots:
    void cardDetected(quint32 nCardId, int nCardType);
public:
    explicit FormEntryReverse(QWidget *parent=0, QLayout *layout=0);
    ~FormEntryReverse();
public:
    QString getMessage();
    void setMessage(const QString &msg);
    void checkAndPromptCardNo();
protected:
     void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *);
private:
    int FCardType;    //要回写卡的卡类型
    QString FCardID;    //要回写卡的卡号
    CARD_TYPE_PHYSICAL m_FPhysicalCardType; // 要回写卡片的物理卡类型
    quint32 m_FPhysicalCardID;  // 要回写卡片的物理序号
private:
    void StopReadCard();

private:
    Ui::FormEntryReverse *ui;
};
FormEntryReverse *getEntryReverse(QWidget *widget = NULL, QLayout* layout = NULL);
#endif // FORMENTRYREVERSE_H
