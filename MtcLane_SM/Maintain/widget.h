#ifndef WIDGET_H
#define WIDGET_H
//#include <maintainlog.h>
#include <QWidget>
#include <maintaincfg.h>
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
//    void on_pushButton_LaneConfig_clicked();

//    void on_pushButton_LogCat_clicked();

//    void on_pushButton_DevTest_clicked();

private:
    Ui::Widget *ui;
    //参数文件配置
    MaintainCFG m_MaintainCfg;
//    MaintainLog m_MaintainLog;
    QStringList Lognamelist;
};

#endif // WIDGET_H
