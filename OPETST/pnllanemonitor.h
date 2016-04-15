#ifndef PNLLANEMONITOR_H
#define PNLLANEMONITOR_H

#include <QWidget>
#include <QMap>
#include "pnlstation.h"
#include "mdichild.h"

namespace Ui {
class CPnlLaneMonitor;
}

class CPnlLaneMonitor : public MdiChild
{
    Q_OBJECT

public:
    explicit CPnlLaneMonitor(QWidget *parent = 0);
    ~CPnlLaneMonitor();

public:
    void InitUI();
    QToolBar* getToolBar(int toolbar) const;
    void createActions();
    void setupDefShortcuts();

    bool isUncommited() const;
    QString getQuitUncommitedConfirmMessage() const;
protected:
    QVariant saveSession();
    bool restoreSession(const QVariant& sessionValue);

    Icon* getIconNameForMdiWindow();
    QString getTitleForMdiWindow();

private slots:
    void ChangeStationSize();

protected:
    void resizeEvent(QResizeEvent * event);

private:
    Ui::CPnlLaneMonitor *ui;
    QMap<int, CPnlStation*> m_pStationMap;
};

#endif // PNLLANEMONITOR_H
