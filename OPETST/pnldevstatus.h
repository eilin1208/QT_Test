#ifndef PNLDEVSTATUS_H
#define PNLDEVSTATUS_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class CPnlDevStatus;
}

class CPnlDevStatus : public QWidget
{
    Q_OBJECT

public:
    explicit CPnlDevStatus(QWidget *parent = 0);
    ~CPnlDevStatus();

public:
    void InitUI(int nStationID,int nLaneID, int nLaneType);

private:
    void AddImageToLbl(QString sImagePath, QLabel *pLable, int nWith,int nHight);

private:
    Ui::CPnlDevStatus *ui;
    int m_nStationID;
    int m_nLaneID;
    int m_nLaneType;
};

#endif // PNLDEVSTATUS_H
