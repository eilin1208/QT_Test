#ifndef FORMPUBLISH_H
#define FORMPUBLISH_H

#include <QWidget>
#include <QTimer>
#include "CommonModule/laneutils_new.h"
namespace Ui {
    class FormPublish;
}

class FormPublish : public QWidget
{
    Q_OBJECT

public:
    explicit FormPublish(QWidget *parent = 0);
    virtual ~FormPublish();
public slots:
    void CheckAndDisplayNew();
private:
    Ui::FormPublish *ui;

};

#endif // FORMPUBLISH_H
