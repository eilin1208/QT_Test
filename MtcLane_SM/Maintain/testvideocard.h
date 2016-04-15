#ifndef TESTVIDEOCARD_H
#define TESTVIDEOCARD_H

#include <QWidget>
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "VideoCardArm/videocardarm.h"
#include <QTimer>
namespace Ui {
    class TestVideoCard;
}

class TestVideoCard : public QWidget
{
    Q_OBJECT

public:
    explicit TestVideoCard(QWidget *parent = 0);
    ~TestVideoCard();

private slots:
    void on_pushButton_capture_clicked();

    void on_pushButton_init_clicked();

    void on_pushButton_close_clicked();

    void AutoCapture(bool clicked);
private:
    Ui::TestVideoCard *ui;
    DevBaseInterface* tmpDev;
    VideoCard_ARM* videocard;
    QString m_filename;
    QTimer *m_timer;
};

#endif // TESTVIDEOCARD_H
