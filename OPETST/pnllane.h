#ifndef PNLLANE_H
#define PNLLANE_H

#include <QWidget>
#include <QLabel>
#include "pnldevstatus.h"
#include "pnllaneinfo.h"

namespace Ui {
class CPnlLane;
}

class CPnlLane : public QWidget
{
    Q_OBJECT

public:
    explicit CPnlLane(QWidget *parent = 0);
    ~CPnlLane();
public:
    void InitUI(int nIndex, int nStationID,int nLaneID, int nLaneType);
    void SpreadLane(bool bSpread);

Q_SIGNALS:
    void spreadLaneInfo();
private slots:
    void on_btnSpread_clicked();

private:
    void SetLaneInfo(int nLaneID, int nLaneType);
    void AddImageToLbl(QString sImagePath, QLabel *pLable, int nWith,int nHight);

private:
    Ui::CPnlLane *ui;
    CPnlDevStatus *m_pnlDevStatus;
    CPnlLaneInfo *m_pnlLaneInfo;
    int m_nStationID;
    int m_nLaneID;
    int m_nLaneType;
};

#endif // PNLLANE_H
