#ifndef FORMMESSAGE_H
#define FORMMESSAGE_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
namespace Ui {
    class FormMessage;
}

class FormMessage : public MtcOpWidget
{
    Q_OBJECT
    Q_PROPERTY(QString message READ getMessage WRITE setMessage)
public:
    explicit FormMessage(QWidget *parent=0, QLayout *layout=0);
    virtual ~FormMessage();
    //properties
public:
    QString getMessage();
    void setMessage(const QString &msg);
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void hideEvent(QHideEvent *);
private:
    void handleConfirmKey();
    void handleEscKey();
private:
    Ui::FormMessage *ui;
};

FormMessage* getMsgWidget(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMMESSAGE_H
