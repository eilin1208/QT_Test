#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include "icon.h"
#include <QStringList>
#include <QHash>
#include <QIcon>
#include <QVariant>

class QMovie;

class IconManager : public QObject
{
    Q_OBJECT

public:
    DEF_ICONS(Icons, iconEnums,
              DEF_ICON(ABORT24,                             "abort24")
              DEF_ICON(LANEMONITOR,                         "LaneMonitor")
              DEF_ICON(QRYLANELOG,                          "log_query")
              DEF_ICON(QRYOPERLOG,                          "log_query")
              DEF_ICON(MESSAGESEND,                         "Message_Send")
              DEF_ICON(REMOTECONTROLS,                      "Remote_Controls")
              DEF_ICON(ACT_CLEAR,                           "act_clear")
              DEF_ICON(ACT_COPY,                            "act_copy")
              DEF_ICON(APP_EXIT,                            "Application-Exit")
              DEF_ICON(CLOSE,                               "close")
              DEF_ICON(CONFIGURE,                           "configure")
              DEF_ICON(FONT_BROWSE,                         "font_browse")
              DEF_ICON(RELOADDB,                            "database_reload")
              DEF_ICON(GO_BACK,                             "go_back")
              DEF_ICON(HELP,                                "help")
              DEF_ICON(HOMEPAGE,                            "homepage")
              DEF_ICON(IMPORT,                              "import")
              DEF_ICON(INDEX,                               "index")
              DEF_ICO2(INDEX_ADD,                           INDEX, PLUS)
              DEF_ICO2(INDEX_DEL,                           INDEX, MINUS)
              DEF_ICO2(INDEX_EDIT,                          INDEX, EDIT)
              DEF_ICON(INDEXES,                             "indexes")
              DEF_ICON(INDICATOR_ERROR,                     "indicator_error")
              DEF_ICON(INDICATOR_HINT,                      "indicator_hint")
              DEF_ICON(INDICATOR_INFO,                      "indicator_info")
              DEF_ICON(INDICATOR_WARN,                      "indicator_warn")
              DEF_ICON(INFO_BALLOON,                        "info_balloon")
              DEF_ICON(LOADING,                             "loading")
              DEF_ICON(OPEN_FILE,                           "open_sql_file")
              DEF_ICON(OPEN_SQL_EDITOR,                     "open_sql_editor")
              DEF_ICON(PLUS,                                "plus")
              DEF_ICON(RELOAD,                              "reload")
              DEF_ICON(SQLITE_DOCS,                         "sqlite_docs")
              DEF_ICON(SYSTEM_LOG_OUT,                      "System-Log-Out")
              DEF_ICON(OPETST_APP,                          "OPETST")
              DEF_ICON(STATUS_ERROR,                        "status_error")
              DEF_ICON(STATUS_INFO,                         "status_info")
              DEF_ICON(STATUS_WARNING,                      "status_warn")
              DEF_ICON(WIN_CASCADE,                         "win_cascade")
              DEF_ICON(WIN_TILE,                            "win_tile")
              DEF_ICON(WIN_TILE_HORIZONTAL,                 "win_tile_horizontal")
              DEF_ICON(WIN_TILE_VERTICAL,                   "win_tile_vertical")
              DEF_ICON(WIN_CLOSE,                           "window_close")
              DEF_ICON(WIN_CLOSE_ALL,                       "window_close_all")
              DEF_ICON(WIN_CLOSE_OTHER,                     "window_close_other")
              DEF_ICON(WIN_RESTORE,                         "window_restore")
              DEF_ICON(WIN_RENAME,                          "window_rename")
              )

    static IconManager* getInstance();

    QString getFilePathForName(const QString& name);
    bool isMovie(const QString& name);
    QMovie* getMovie(const QString& name);
    QIcon* getIcon(const QString& name);
    void init();
    QStringList getIconDirs() const;

private:
    IconManager();
    void loadRecurently(QString dirPath, const QString& prefix, bool movie);
    void enableRescanning();

    static IconManager* instance;
    QHash<QString,QIcon*> icons;
    QHash<QString,QMovie*> movies;
    QHash<QString,QString> paths;
    QStringList iconDirs;
    QStringList iconFileExtensions;
    QStringList movieFileExtensions;
    QStringList resourceIcons;
    QStringList resourceMovies;

public slots:
    void rescanResources();
};

#define ICONMANAGER IconManager::getInstance()
#define ICONS ICONMANAGER->iconEnums

#endif // ICONMANAGER_H
