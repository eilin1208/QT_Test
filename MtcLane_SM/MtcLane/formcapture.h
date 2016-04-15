#ifndef FORMCAPTURE_H
#define FORMCAPTURE_H

#include <QWidget>

namespace Ui {
    class FormCapture;
}

class FormCapture : public QWidget
{
    Q_OBJECT

public:
    explicit FormCapture(QWidget *parent = 0);
    virtual ~FormCapture();

    //获取视频显示的Widget
    QWidget* getVideoCardWidget();

private:
    Ui::FormCapture *ui;
};

#endif // FORMCAPTURE_H
