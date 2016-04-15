#ifndef FORMVEHQUEBEGIN_H
#define FORMVEHQUEBEGIN_H

#include <QWidget>
#include <MtcKey/MtcOpWidget.h>
#include "laneinfo.h"
namespace Ui {
class FormVehQueBegin;
}

class FormVehQueBegin : public MtcOpWidget
{
    Q_OBJECT

public:
    explicit FormVehQueBegin(QWidget *parent = 0,QLayout *layout =0);
    virtual ~FormVehQueBegin();
    void resetVehcount(int count);

public slots:
    void setVehCount();
    int getVehCount()
    {
        return vehCount;
    }

    void clearVehCount();
protected:
    void showEvent(QShowEvent *);
private:
    Ui::FormVehQueBegin *ui;
    int vehCount;
};
FormVehQueBegin * getQueBegin();
#endif // FORMVEHQUEBEGIN_H
