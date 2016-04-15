#ifndef FORMENTRYPLATE_H
#define FORMENTRYPLATE_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "LocalCfgDb/LocalCfgDb.h"

namespace Ui {
class FormEntryPlate;
}

class FormEntryPlate : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit FormEntryPlate(QWidget *parent = 0, QLayout * layout = 0);
    virtual ~FormEntryPlate();
    void getPlateInfo(int &nColor, QString& sPlate);
    //void setPlateInfo(int nColor, const QString& sPlate);
public slots:
    void getVprResult(int nIndex);
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void handleVehColorKey(MtcKeyPressedEvent* mtcKeyEvent);
    void showLetterResult(const QStringList& stringList);
    void inputPlate(MtcKeyPressedEvent *mtcKeyEvent);
    void moveandDelInput(MtcKeyPressedEvent *mtcKeyEvent);
    void GetCharacter(QString szPinYin);
    void showEvent(QShowEvent *event);
    void showBold();
//    void hideEvent(QHideEvent *);
private:
    Ui::FormEntryPlate *ui;
    QStringList characterList;
    QString m_inPut;
    QLocalDbCfg m_db;
    int m_nIndex;
public:
    void ResetShow();

};
FormEntryPlate* getShowEnPlate(QWidget *widget = NULL, QLayout *layout = NULL);
#endif // FORMENTRYPLATE_H
