#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QWidget>
#include "mdichild.h"

namespace Ui {
class EditorWindow;
}

class EditorWindow : public MdiChild
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();

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
private:
    Ui::EditorWindow *ui;
    int sqlEditorNum = 1;
};

#endif // EDITORWINDOW_H
