#ifndef FORMLOGIN_H
#define FORMLOGIN_H

#include <QWidget>

namespace Ui {
    class FormLogin;
}

class FormLogin : public QWidget
{
    Q_OBJECT

public:
    explicit FormLogin(QWidget *parent = 0);
    virtual ~FormLogin();

private:
    Ui::FormLogin *ui;
};

#endif // FORMLOGIN_H
