#ifndef FORMSHOWWEIGHT_H
#define FORMSHOWWEIGHT_H

#include <QWidget>

namespace Ui {
    class Formshowweight;
}

class Formshowweight : public QWidget
{
    Q_OBJECT

public:
    explicit Formshowweight(QWidget *parent = 0);
    virtual ~Formshowweight();

private:
    Ui::Formshowweight *ui;
};

#endif // FORMSHOWWEIGHT_H
