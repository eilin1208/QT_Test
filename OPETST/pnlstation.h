#ifndef PNLSTATION_H
#define PNLSTATION_H

#include <QWidget>
#include <QMap>
#include <QSpacerItem>
#include "pnllane.h"

namespace Ui {
class CPnlStation;
}

class CPnlStation : public QWidget
{
    Q_OBJECT

public:
    explicit CPnlStation(QWidget *parent = 0);
    ~CPnlStation();

public:
    void InitUI(int nOrgCode);

    int GetUIHeight();

Q_SIGNALS:
    void ChangeStationSize();

private slots:
    void spreadLaneInfo();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

protected:
    void resizeEvent(QResizeEvent * event);

private:
    Ui::CPnlStation *ui;
    QSpacerItem *verticalSpacer;
    QMap<int, CPnlLane*> m_pLaneMap;
};

#endif // PNLSTATION_H
