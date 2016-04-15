#ifndef FORMBOTTOMBAR_H
#define FORMBOTTOMBAR_H

#include <QWidget>

namespace Ui {
    class FormBottomBar;
}

class FormBottomBar : public QWidget
{
    Q_OBJECT

public:
    explicit FormBottomBar(QWidget *parent = 0);
    virtual ~FormBottomBar();
    void setRateVersion(const QString& sVersion);
    void ShowUnUpWaste();
    void ShowDiskSize();
    void showOperNo(quint32 dwOperNo);
    void showMemoryInfo();
protected:
    void timerEvent(QTimerEvent *event);
private:
private:
    Ui::FormBottomBar *ui;
    int m_nShowTimer;
    int m_nUnUpLoadTimer;
    int m_nShowDiskSize;
};

#endif // FORMBOTTOMBAR_H
