#include "mainwindow.h"
#include "mdiarea.h"
#include "common/extaction.h"
#include "editorwindow.h"
#include "configdialog.h"
#include "statusfield.h"
#include "lanewaste.h"
#include "common/notifymanager.h"
#include "pnllanemonitor.h"
#include "themetuner.h"
#include "config_builder/configimpl.h"
#include "uiconfig.h"

#include <QInputDialog>
#include <QDebug>
#include <QStyleFactory>

static const int sqlitestudioVersion = 10000;

//CFG_KEYS_DEFINE(MainWindow)
MainWindow* MainWindow::instance = nullptr;

MainWindow::MainWindow(/*QWidget *parent*/) :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    CfgMain::staticInit();
    CFG->init();
    //restoreSession();
    ui->setupUi(this);

    this->setWindowState(Qt::WindowMaximized);

#ifdef Q_OS_WIN
    setWindowIcon(ICONS.OPETST_APP.toQIcon().pixmap(256, 256));
#else
    setWindowIcon(ICONS.OPETST_APP);
#endif

    setWindowTitle(QString("Toll Supervision Terminal (Ver:%1)").arg(getVersionString()));

    updateCornerDocking();
    setDockOptions(QMainWindow::ForceTabbedDocks);

    LaneWaste *pLaneWaste = new LaneWaste(this);
    addDockWidget(Qt::BottomDockWidgetArea, pLaneWaste);

    statusField = new StatusField(this);
    addDockWidget(Qt::BottomDockWidgetArea, statusField);

    tabifyDockWidget(pLaneWaste, statusField);

    initActions();
    ui->mdiArea->setTaskBar(ui->taskBar);
    addToolBar(Qt::BottomToolBarArea, ui->taskBar);

    initMenuBar();

    updateWindowActions();
    qApp->installEventFilter(this);

}

void MainWindow::closeEvent(QCloseEvent* event)
{
//    if (SQLITESTUDIO->getImmediateQuit())
//    {
//        closingApp = true;
//        QMainWindow::closeEvent(event);
//        return;
//    }

    if (!Committable::canQuit())
    {
//        event->ignore();
        return;
    }

    closingApp = true;
    closeNonSessionWindows();
//    MdiWindow* currWindow = ui->mdiArea->getCurrentWindow();
    hide();
    QMainWindow::closeEvent(event);
}

void MainWindow::closeNonSessionWindows()
{
    foreach (MdiWindow* window, ui->mdiArea->getWindows())
        if (!window->restoreSessionNextTime())
            window->close();
}

int MainWindow::getVersion() const
{
    return sqlitestudioVersion;
}

QString MainWindow::getVersionString() const
{
    int ver = getVersion();
    int majorVer = ver / 10000;
    int minorVer = ver % 10000 / 100;
    int patchVer = ver % 100;
    return QString::number(majorVer) + "." + QString::number(minorVer) + "." + QString::number(patchVer);
}

void MainWindow::initMenuBar()
{
    //systemToolMenu
    systemToolMenu = new QMenu(this);
    systemToolMenu->setTitle(tr("System", "menubar"));
    menuBar()->addMenu(systemToolMenu);

    systemToolMenu->addAction(actionMap[RELOADDB]);
    systemToolMenu->addSeparator();
    systemToolMenu->addAction(actionMap[SYSTEM_LOGOUT]);
    systemToolMenu->addAction(actionMap[APP_EXIT]);

    //mainToolMenu
    mainToolMenu = new QMenu(this);
    mainToolMenu->setTitle(tr("Oper", "menubar"));
    menuBar()->addMenu(mainToolMenu);
    mainToolMenu->addAction(actionMap[LANEMONITOR]);
    mainToolMenu->addAction(actionMap[MESSAGESEND]);
    mainToolMenu->addAction(actionMap[REMOTECONTROLS]);
    mainToolMenu->addSeparator();
    mainToolMenu->addAction(actionMap[QRYLANELOG]);
    mainToolMenu->addAction(actionMap[QRYOPERLOG]);

    // View menu
    viewMenu = createPopupMenu();
    viewMenu->setTitle(tr("View", "menubar"));
    menuBar()->addMenu(viewMenu);

    mdiMenu = new QMenu(viewMenu);
    mdiMenu->setTitle(tr("Window list", "menubar view menu"));
    connect(ui->mdiArea, &MdiArea::windowListChanged, this, &MainWindow::refreshMdiWindows);

    viewMenu->addSeparator();
    viewMenu->addAction(actionMap[MDI_TILE]);
    viewMenu->addAction(actionMap[MDI_TILE_HORIZONTAL]);
    viewMenu->addAction(actionMap[MDI_TILE_VERTICAL]);
    viewMenu->addAction(actionMap[MDI_CASCADE]);
    viewMenu->addSeparator();
    viewMenu->addAction(actionMap[CLOSE_WINDOW]);
    viewMenu->addAction(actionMap[CLOSE_OTHER_WINDOWS]);
    viewMenu->addAction(actionMap[CLOSE_ALL_WINDOWS]);
    viewMenu->addSeparator();
    viewMenu->addAction(actionMap[RESTORE_WINDOW]);
    viewMenu->addAction(actionMap[RENAME_WINDOW]);

    viewMenu->addSeparator();
    viewMenu->addMenu(mdiMenu);

    //helpMenu
    helpMenu = new QMenu(this);
    helpMenu->setTitle(tr("Help", "menubar"));
    menuBar()->addMenu(helpMenu);
}

MainWindow *MainWindow::getInstance()
{
    if(!instance)
        instance = new MainWindow();

    return instance;
}

void MainWindow::refreshMdiWindows()
{
    mdiMenu->clear();

    foreach (QAction* action, getMdiArea()->getTaskBar()->getTasks())
        mdiMenu->addAction(action);

    updateWindowActions();
}

bool MainWindow::isClosingApp() const
{
    return closingApp;
}

QToolBar* MainWindow::getToolBar(int toolbar) const
{
    switch (static_cast<ToolBar>(toolbar))
    {
        case TOOLBAR_MAIN:
            return ui->mainToolBar;
        case TOOLBAR_TOOLS:
            return ui->systemToolBar;
        case TOOLBAR_VIEW:
            return ui->viewToolbar;
    }
    return nullptr;
}

void MainWindow::createActions()
{
    createAction(LANEMONITOR, ICONS.LANEMONITOR, tr("Lane Monitor"), this, SLOT(OpenLaneMonitor()), this, ui->mainToolBar);
    createAction(QRYLANELOG, ICONS.QRYLANELOG, tr("Lane Log Query"), this, SLOT(on_action122222_triggered()), this, ui->mainToolBar);
    createAction(QRYOPERLOG, ICONS.QRYOPERLOG, tr("Oper Log Query"), this, SLOT(ReLoadDatabase()), this, ui->mainToolBar);
    createAction(MESSAGESEND, ICONS.MESSAGESEND, tr("Message Send"), this, SLOT(ReLoadDatabase()), this, ui->mainToolBar);
    createAction(REMOTECONTROLS, ICONS.REMOTECONTROLS, tr("Remote Controls"), this, SLOT(ReLoadDatabase()), this, ui->mainToolBar);

    createAction(RELOADDB, ICONS.RELOADDB, tr("Refresh databse"), this, SLOT(ReLoadDatabase()), this, ui->systemToolBar);
    createAction(SYSTEM_LOGOUT, ICONS.SYSTEM_LOG_OUT, tr("Logout"), this, SLOT(SystemLogout()), this, ui->systemToolBar);
    createAction(APP_EXIT, ICONS.APP_EXIT, tr("Exit"), this, SLOT(AppExit()), this, ui->systemToolBar);

    createAction(MDI_TILE, ICONS.WIN_TILE, tr("Tile windows"), ui->mdiArea, SLOT(tileSubWindows()), ui->viewToolbar);
    createAction(MDI_TILE_HORIZONTAL, ICONS.WIN_TILE_HORIZONTAL, tr("Tile windows horizontally"), ui->mdiArea, SLOT(tileHorizontally()), ui->viewToolbar);
    createAction(MDI_TILE_VERTICAL, ICONS.WIN_TILE_VERTICAL, tr("Tile windows vertically"), ui->mdiArea, SLOT(tileVertically()), ui->viewToolbar);
    createAction(MDI_CASCADE, ICONS.WIN_CASCADE, tr("Cascade windows"), ui->mdiArea, SLOT(cascadeSubWindows()), ui->viewToolbar);

    createAction(CLOSE_WINDOW, ICONS.WIN_CLOSE, tr("Close selected window"), this, SLOT(closeSelectedWindow()), this);
    createAction(CLOSE_OTHER_WINDOWS, ICONS.WIN_CLOSE_OTHER, tr("Close all windows but selected"), this, SLOT(closeAllWindowsButSelected()), this);
    createAction(CLOSE_ALL_WINDOWS, ICONS.WIN_CLOSE_ALL, tr("Close all windows"), this, SLOT(closeAllWindows()), this);
    createAction(RESTORE_WINDOW, ICONS.WIN_RESTORE, tr("Restore recently closed window"), this, SLOT(restoreLastClosedWindow()), this);
    createAction(RENAME_WINDOW, ICONS.WIN_RENAME, tr("Rename selected window"), this, SLOT(renameWindow()), this);


    ui->taskBar->initContextMenu(this);
}

void MainWindow::setupDefShortcuts()
{
    //BIND_SHORTCUTS(MainWindow, Action);
}

void MainWindow::updateWindowActions()
{
    bool hasActiveTask = ui->mdiArea->activeSubWindow();
    actionMap[MDI_CASCADE]->setEnabled(hasActiveTask);
    actionMap[MDI_TILE]->setEnabled(hasActiveTask);
    actionMap[MDI_TILE_HORIZONTAL]->setEnabled(hasActiveTask);
    actionMap[MDI_TILE_VERTICAL]->setEnabled(hasActiveTask);
    actionMap[CLOSE_WINDOW]->setEnabled(hasActiveTask);
    actionMap[CLOSE_OTHER_WINDOWS]->setEnabled(hasActiveTask);
    actionMap[CLOSE_ALL_WINDOWS]->setEnabled(hasActiveTask);
    actionMap[RENAME_WINDOW]->setEnabled(hasActiveTask);
    actionMap[RESTORE_WINDOW]->setEnabled(hasClosedWindowToRestore());
}

MdiArea *MainWindow::getMdiArea() const
{
    return dynamic_cast<MdiArea*>(ui->mdiArea);
}

bool MainWindow::hasClosedWindowToRestore() const
{
    return closedWindowSessionValues.size() > 0;
}

void MainWindow::on_action122222_triggered()
{
    EditorWindow* win = new EditorWindow(ui->mdiArea);
    win->SetMdiChildID(1);
    if (win->isInvalid())
    {
        delete win;
        //return nullptr;
    }

    ui->mdiArea->addSubWindow(win);
    //return win;
}


void MainWindow::pushClosedWindowSessionValue(const QVariant &value)
{
    closedWindowSessionValues.enqueue(value);

    if (closedWindowSessionValues.size() > closedWindowsStackSize)
        closedWindowSessionValues.dequeue();
}

void MainWindow::restoreLastClosedWindow()
{
    if (closedWindowSessionValues.size() == 0)
        return;

    QMdiSubWindow* activeWin = ui->mdiArea->activeSubWindow();
    bool maximizedMode = activeWin && activeWin->isMaximized();

    QVariant winSession = closedWindowSessionValues.takeLast();
    if (maximizedMode)
    {
        QHash<QString, QVariant> winSessionHash = winSession.toHash();
        winSessionHash.remove("geometry");
        winSession = winSessionHash;
    }

    restoreWindowSession(winSession);
}

void MainWindow::renameWindow()
{
    MdiWindow* win = ui->mdiArea->getActiveWindow();
    if (!win)
        return;

    QString newTitle = QInputDialog::getText(this, tr("Rename window"), tr("Enter new name for the window:"), QLineEdit::Normal, win->windowTitle());
    if (newTitle == win->windowTitle() || newTitle.isEmpty())
        return;

    win->rename(newTitle);
}

void MainWindow::closeSelectedWindow()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::closeAllWindowsButSelected()
{
    ui->mdiArea->closeAllButActive();
}

void MainWindow::closeAllWindows()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::restoreWindowSessions(const QList<QVariant>& windowSessions)
{
    if (windowSessions.size() == 0)
        return;

    foreach (const QVariant& winSession, windowSessions)
        restoreWindowSession(winSession);
}

MdiWindow* MainWindow::restoreWindowSession(const QVariant &windowSessions)
{
    QHash<QString, QVariant> winSessionHash = windowSessions.toHash();
    if (!winSessionHash.contains("class"))
        return nullptr;

    // Find out the type of stored session
    QByteArray classBytes = winSessionHash["class"].toString().toLatin1();
    char* className = classBytes.data();
    int type = QMetaType::type(className);
    if (type == QMetaType::UnknownType)
    {
        qWarning() << "Could not restore window session, because type" << className
                   << "is not known to Qt meta subsystem.";
        return nullptr;
    }

    // Try to instantiate the object
    void* object = QMetaType::create(type);
    if (!object)
    {
        qWarning() << "Could not restore window session, because type" << className
                   << "could not be instantiated.";
        return nullptr;
    }

    // Switch to session aware window, so we can use its session aware interface.
    MdiChild* mdiChild = reinterpret_cast<MdiChild*>(object);
    if (mdiChild->isInvalid())
    {
        delete mdiChild;
        return nullptr;
    }

    // Add the window to MDI area and restore its session
    MdiWindow* window = ui->mdiArea->addSubWindow(mdiChild);
    if (!window->restoreSession(winSessionHash))
    {
        window->setCloseWithoutSessionSaving(true);
        delete window;
    }

    return window;
}

void MainWindow::on_action1_triggered()
{
    ConfigDialog config(this);
    config.exec();
}

void MainWindow::ReLoadDatabase()
{
    ConfigDialog config(this);
    config.exec();
}


void MainWindow::AppExit()
{
    this->close();
}

void MainWindow::SystemLogout()
{

}


void MainWindow::OpenLaneMonitor()
{
    MdiWindow* mdiWin = ui->mdiArea->GetMdiChid(LANEMONITOR);
    if (mdiWin)
    {
        ui->mdiArea->setActiveSubWindow(mdiWin);
        return;
    }

    CPnlLaneMonitor* win = new CPnlLaneMonitor(ui->mdiArea);
    win->InitUI();
    win->SetMdiChildID(LANEMONITOR);
    if (win->isInvalid())
    {
        delete win;
        //return nullptr;
    }
    ui->mdiArea->addSubWindow(win);
}


QString MainWindow::currentStyle() const
{
    return QApplication::style()->objectName();
}

void MainWindow::setStyle(const QString& styleName)
{
    QStyle* style = QStyleFactory::create(styleName);
    if (!style)
    {
        notifyWarn(tr("Could not set style: %1", "main window").arg(styleName));
        return;
    }
    QApplication::setStyle(style);
    THEME_TUNER->tuneTheme(styleName);
}

void MainWindow::restoreSession()
{
    //return;
    QHash<QString,QVariant> sessionValue = CFG_UI.General.Session.get();
    if (sessionValue.size() == 0)
    {
        THEME_TUNER->tuneCurrentTheme();
        return;
    }

    if (sessionValue.contains("style"))
        setStyle(sessionValue["style"].toString());
    else
        THEME_TUNER->tuneCurrentTheme();

    if (sessionValue.contains("geometry"))
        restoreGeometry(sessionValue["geometry"].toByteArray());

    if (sessionValue.contains("state"))
        restoreState(sessionValue["state"].toByteArray());

    if (CFG_UI.General.RestoreSession.get())
    {
        if (sessionValue.contains("windowSessions"))
            restoreWindowSessions(sessionValue["windowSessions"].toList());

        if (sessionValue.contains("activeWindowTitle"))
        {
            QString title = sessionValue["activeWindowTitle"].toString();
            MdiWindow* window = ui->mdiArea->getWindowByTitle(title);
            if (window)
                ui->mdiArea->setActiveSubWindow(window);
        }
    }

    if (statusField->hasMessages())
        statusField->setVisible(true);

    updateCornerDocking();
    updateWindowActions();
}

void MainWindow::updateCornerDocking()
{
    if (CFG_UI.General.DockLayout.get() == "vertical") {
        setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
        setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
        setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
        setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    } else {
        setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
        setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
        setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
        setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);
    }
}
