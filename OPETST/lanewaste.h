#ifndef LANEWASTE_H
#define LANEWASTE_H

#include <QDockWidget>

namespace Ui {
class LaneWaste;
}

class LaneWaste : public QDockWidget
{
    Q_OBJECT

public:
    explicit LaneWaste(QWidget *parent = 0);
    ~LaneWaste();

private:
    Ui::LaneWaste *ui;
};

#endif // LANEWASTE_H
