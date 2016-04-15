

#ifndef FORMFUNCTION_H
#define FORMFUNCTION_H

#include <QWidget>
#include <MtcKey/MtcOpWidget.h>
#include "BakupModule/bakupdatamgr.h"
namespace Ui {
class FormFunction;
}

class FormFunction : public MtcOpWidget
{
    Q_OBJECT
    enum FUNCSTATUS
    {
        DETECT,
        WAIT,
        FULL,
        EXEC,
        SUCCESS,
        FAIL
    };

public:
    explicit FormFunction(QWidget *parent = 0, QLayout* layout = 0);
    ~FormFunction();
    void setFuncType(int nType);
public slots:
    void devActive(int nStatus);
    void resultSuccess();
    void resultFail();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *event);
    void showEvent(QShowEvent *);
private:
    bool isAllowEsc()
    {
        return m_nStatus != EXEC;
    }
    bool isWaiting()
    {
        return m_nStatus == WAIT;
    }

    bool isFinish()
    {
        return m_nStatus >= SUCCESS;
    }
    void showNote(FUNCSTATUS status);
    void beginBakup();
private:
    Ui::FormFunction *ui;
    int m_nType;
    BakupDataMgr m_bakUpMgr;
    int m_nStatus;
};

FormFunction* getFuncForm(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMFUNCTION_H
