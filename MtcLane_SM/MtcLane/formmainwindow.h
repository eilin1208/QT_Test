#ifndef FORMMIANWINDOW_H
#define FORMMIANWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include "formwarning.h"
#include <QProcess>
namespace Ui {
class FormMianWindow;
}
enum FormType
{
    emFormTitle,              //标题窗口
    emFormCapture,            //抓拍窗口
    emFormVpr,                //车牌识别
    emFormBottomBar,          //状态栏窗口
    emFormDevice,              //设备窗口
    emFormWtInfo,              //记重信息
    emFormRecordTable,        //收费记录窗口
    emFormPublish,          //公告窗口
    emFormWarning,           //提示窗口
    emFormCardMgr,          //卡机窗口
    emFormLog               //日志窗口
};
class FormMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FormMainWindow(QWidget *parent = 0);
    virtual ~FormMainWindow();
    QWidget* getWidgetByType(FormType type);
    QLayout* getStateWidgetLayout()
    {
        return m_leftLayout;
    }
protected:
    void paintEvent(QPaintEvent *);
    void buildWindows();
    void buildEntryWindows(QWidget* leftWidget, QWidget* rightWidget);
    void buildExitWindows(QWidget* leftWidget, QWidget* rightWidget);
    void buildAutoWindows(QWidget* leftWidget, QWidget* rightWidget);
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
private:
    Ui::FormMianWindow *ui;
    QMap< FormType, QWidget* > m_widgetMap;
    QVBoxLayout* m_leftLayout;
    int m_nCloseTimer;
    //
private slots:
    void ShowFree()
    {
        //QProcess::execute("free > a.txt");
    }
};
QWidget* getMainWindow();
bool getWidgetByType(FormType type, QWidget** widget);
void showWarning(const QString& sWarning, bool bOpenTimer = true, FormWarning::WARNINGTYPE type = FormWarning::MESSAGE);
#endif // FORMMIANWINDOW_H
