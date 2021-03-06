#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "common/extactioncontainer.h"
#include "ui_mainwindow.h"
#include "mdiwindow.h"

#include <QQueue>
#include <QMainWindow>

class MdiArea;
class MdiWindow;
class StatusField;
class ThemeTuner;


#ifdef Q_OS_MACX
#define PREV_TASK_KEY_SEQ Qt::CTRL + Qt::ALT + Qt::Key_Left
#define NEXT_TASK_KEY_SEQ Qt::CTRL + Qt::ALT + Qt::Key_Right
#else
#define PREV_TASK_KEY_SEQ Qt::CTRL + Qt::Key_PageUp
#define NEXT_TASK_KEY_SEQ Qt::CTRL + Qt::Key_PageDown
#endif

CFG_KEY_LIST(MainWindow, QObject::tr("Main window"),
     CFG_KEY_ENTRY(OPEN_SQL_EDITOR,    Qt::ALT + Qt::Key_E,         QObject::tr("Open SQL editor"))
     CFG_KEY_ENTRY(PREV_TASK,          PREV_TASK_KEY_SEQ,           QObject::tr("Previous window"))
     CFG_KEY_ENTRY(NEXT_TASK,          NEXT_TASK_KEY_SEQ,           QObject::tr("Next window"))
     CFG_KEY_ENTRY(HIDE_STATUS_FIELD,  Qt::Key_Escape,              QObject::tr("Hide status area"))
     CFG_KEY_ENTRY(OPEN_CONFIG,        Qt::Key_F2,                  QObject::tr("Open configuration dialog"))
     CFG_KEY_ENTRY(OPEN_DEBUG_CONSOLE, Qt::Key_F12,                 QObject::tr("Open Debug Console"))
     CFG_KEY_ENTRY(OPEN_CSS_CONSOLE,   Qt::Key_F11,                 QObject::tr("Open CSS Console"))
)

class MainWindow : public QMainWindow, public ExtActionContainer
{
    Q_OBJECT
    Q_ENUMS(Action)

public:
    enum Action
    {
        LANEMONITOR,
        QRYLANELOG,
        QRYOPERLOG,
        MESSAGESEND,
        REMOTECONTROLS,
        RELOADDB,
        SYSTEM_LOGOUT,
        APP_EXIT,
        MDI_TILE,
        MDI_CASCADE,
        MDI_TILE_HORIZONTAL,
        MDI_TILE_VERTICAL,
        CLOSE_WINDOW,
        CLOSE_ALL_WINDOWS,
        CLOSE_OTHER_WINDOWS,
        RESTORE_WINDOW,
        RENAME_WINDOW,
        END
    };
    enum ToolBar
    {
        TOOLBAR_MAIN,
        TOOLBAR_TOOLS,
        TOOLBAR_VIEW
    };

private:
    MainWindow(/*QWidget *parent = 0*/);
    ~MainWindow();

    void init();
    void createActions();
    void setupDefShortcuts();
    void initMenuBar();
    void restoreWindowSessions(const QList<QVariant>& windowSessions);
    MdiWindow *restoreWindowSession(const QVariant& windowSessions);
    void closeNonSessionWindows();

public:
    static MainWindow *getInstance();

    int getVersion() const;
    QString getVersionString() const;

    void restoreSession();

    void pushClosedWindowSessionValue(const QVariant& value);
    bool hasClosedWindowToRestore() const;
    QToolBar* getToolBar(int toolbar) const;


    MdiArea* getMdiArea() const;
    bool isClosingApp() const;

    QString currentStyle() const;
    void setStyle(const QString& styleName);

    template <class T>
    T* openMdiWindow();
    Ui::MainWindow *ui = nullptr;

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void updateWindowActions();
    void updateCornerDocking();

private slots:
    void ReLoadDatabase();
    void AppExit();
    void SystemLogout();
    void on_action122222_triggered();
    void closeAllWindows();
    void closeAllWindowsButSelected();
    void closeSelectedWindow();
    void restoreLastClosedWindow();
    void renameWindow();
    void refreshMdiWindows();
    void OpenLaneMonitor();

    void on_action1_triggered();

private:
    static MainWindow* instance;
    bool closingApp = false;
    QQueue<QVariant> closedWindowSessionValues;
    static constexpr int closedWindowsStackSize = 20;
    QMenu* systemToolMenu = nullptr;
    QMenu* mainToolMenu = nullptr;
    QMenu* viewMenu = nullptr;
    QMenu* mdiMenu = nullptr;
    QMenu* helpMenu = nullptr;
    StatusField* statusField = nullptr;
};

template <class T>
T* MainWindow::openMdiWindow()
{
    T* win = nullptr;
    foreach (MdiWindow* mdiWin, ui->mdiArea->getWindows())
    {
        win = dynamic_cast<T*>(mdiWin->getMdiChild());
        if (win)
        {
            ui->mdiArea->setActiveSubWindow(mdiWin);
            return win;
        }
    }

    win = new T(ui->mdiArea);
    if (win->isInvalid())
    {
        delete win;
        return nullptr;
    }

    ui->mdiArea->addSubWindow(win);
    return win;
}

#define MAINWINDOW MainWindow::getInstance()
#endif // MAINWINDOW_H
