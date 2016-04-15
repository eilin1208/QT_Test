#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class CardMgrArm;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();
    void cmPush(int index);
    void cmCardOut(int index);
    void cmCardAway(int index);
    void cmUpdate();
    void showLog(const QString &sLog);
private:
    void loadDriver();
private:
    Ui::Widget *ui;
    CardMgrArm* m_cardMgr;
    //初始化是否成功
    bool m_isInit;
};

#endif // WIDGET_H
