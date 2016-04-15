#ifndef DIALOGSHOWPROINFO_H
#define DIALOGSHOWPROINFO_H

#include <QDialog>
#include "CommonModule/LaneType.h"
#include "MtcKey/MtcOpWidget.h"
#include "exittollitem.h"

namespace Ui {
class DialogShowProInfo;
}

class DialogShowProInfo : public MtcOpWidget
{
    Q_OBJECT
    
public:
    explicit DialogShowProInfo(QWidget *parent = 0, QLayout* layout = 0);
    ~DialogShowProInfo();
protected:
    void showProInfo();
    void clearProInfo();
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    bool doPay();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private:
    Ui::DialogShowProInfo *ui;
    TProCardConsumeInfo m_proCardConsumeInfo;
};

DialogShowProInfo* getShowProInfo(QWidget *widget = NULL, QLayout* layout = NULL);

#endif // DIALOGSHOWPROINFO_H
