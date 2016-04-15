#ifndef PNLLANEINFO_H
#define PNLLANEINFO_H

#include <QWidget>

namespace Ui {
class CPnlLaneInfo;
}

class CPnlLaneInfo : public QWidget
{
    Q_OBJECT

public:
    explicit CPnlLaneInfo(QWidget *parent = 0);
    ~CPnlLaneInfo();

private:
    Ui::CPnlLaneInfo *ui;
};

#endif // PNLLANEINFO_H
