#ifndef FORMCARDMGR_H
#define FORMCARDMGR_H

#include <QWidget>
#include <QCheckBox>
#include <QMap>
class QLabel;
namespace Ui {
class FormCardMgr;
}

class FormCardMgr : public QWidget
{
    Q_OBJECT
public:
    explicit FormCardMgr(QWidget *parent = 0);
    ~FormCardMgr();
public slots:
    void changeCardReader(int status);
    void changeButton(int pos);
    //卡机出现异常
    void cmException(int index);
    //卡机恢复正常
    void cmNormal(int index);
    //刷新信息
    void refreshInfo();
protected:
    void timerEvent(QTimerEvent *);
private slots:

    void on_CardBox1ModifyButton_clicked();

    void on_CardBox3ModifyButton_clicked();

    void on_CardBox2ModifyButton_clicked();

    void on_CardBox4ModifyButton_clicked();

private:
    Ui::FormCardMgr *ui;
    int m_pushTimer;
    QLabel* m_curLabel;
    QMap<int, QCheckBox*> m_mapStatus;
};

#endif // FORMCARDMGR_H
