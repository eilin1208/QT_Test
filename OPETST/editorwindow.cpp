#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "mainwindow.h"


EditorWindow::EditorWindow(QWidget *parent) :
    MdiChild(parent),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);
//    createDbCombo();
    initActions();
    updateShortcutTips();
}

EditorWindow::~EditorWindow()
{
    delete ui;
}


QToolBar* EditorWindow::getToolBar(int toolbar) const
{
    return nullptr;
}

void EditorWindow::createActions()
{

}

void EditorWindow::setupDefShortcuts()
{

}

QVariant EditorWindow::saveSession()
{
    return 0;
}

bool EditorWindow::restoreSession(const QVariant& sessionValue)
{
    return true;
}

Icon* EditorWindow::getIconNameForMdiWindow()
{
    return ICONS.OPEN_SQL_EDITOR;
}

QString EditorWindow::getTitleForMdiWindow()
{
    QStringList existingNames = MainWindow::getInstance()->getMdiArea()->getWindowTitles();
    QString title = tr("SQL editor %1").arg(sqlEditorNum++);
    while (existingNames.contains(title))
        title = tr("SQL editor %1").arg(sqlEditorNum++);

    return title;
}


bool EditorWindow::isUncommited() const
{
    return false;
}

QString EditorWindow::getQuitUncommitedConfirmMessage() const
{
    return tr("Editor window \"%1\" has uncommited data.").arg(getMdiWindow()->windowTitle());
}
