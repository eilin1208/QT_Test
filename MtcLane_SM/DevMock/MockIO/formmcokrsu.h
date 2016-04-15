#ifndef FORMMCOKRSU_H
#define FORMMCOKRSU_H

#include <QWidget>

namespace Ui {
    class FormMcokRsu;
}

class FormMcokRsu : public QWidget
{
    Q_OBJECT

public:
    explicit FormMcokRsu(QWidget *parent = 0);
    ~FormMcokRsu();

private:
    Ui::FormMcokRsu *ui;
};

#endif // FORMMCOKRSU_H
