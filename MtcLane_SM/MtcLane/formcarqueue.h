#ifndef FORMCARQUEUE_H
#define FORMCARQUEUE_H

#include <QWidget>

namespace Ui {
    class FormCarQueue;
}

class FormCarQueue : public QWidget
{
    Q_OBJECT

public:
    explicit FormCarQueue(QWidget *parent = 0);
    virtual ~FormCarQueue();

private:
    Ui::FormCarQueue *ui;
};

#endif // FORMCARQUEUE_H
