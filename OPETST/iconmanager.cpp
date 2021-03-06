#include "iconmanager.h"
#include "common/unused.h"
#include "common/global.h"
#include <QApplication>
#include <QDir>
#include <QString>
#include <QIcon>
#include <QMovie>
#include <QDebug>
#include <QPainter>

IconManager* IconManager::instance = nullptr;

IconManager* IconManager::getInstance()
{
    if (instance == nullptr)
        instance = new IconManager();

    return instance;
}

QString IconManager::getFilePathForName(const QString& name)
{
    return paths[name];
}

IconManager::IconManager()
{
}

void IconManager::init()
{
    Icon::init();

    iconDirs += qApp->applicationDirPath() + "/img";
    iconDirs += ":/icons";

#ifdef ICONS_DIR
    iconDirs += STRINGIFY(ICONS_DIR);
#endif

    iconFileExtensions << "*.png" << "*.PNG" << "*.jpg" << "*.JPG" << "*.svg" << "*.SVG";
    movieFileExtensions << "*.gif" << "*.GIF" << "*.mng" << "*.MNG";

    foreach (QString dirPath, iconDirs)
    {
        loadRecurently(dirPath, "", false);
        loadRecurently(dirPath, "", true);
    }

    Icon::loadAll();
}

QStringList IconManager::getIconDirs() const
{
    return iconDirs;
}

void IconManager::rescanResources()
{
    for (const QString& name : resourceMovies)
    {
        delete movies[name];
        movies.remove(name);
    }

    for (const QString& name : resourceIcons)
        icons.remove(name);

    resourceMovies.clear();
    resourceIcons.clear();
    loadRecurently(":/icons", "", true);
    loadRecurently(":/icons", "", false);

    Icon::reloadAll();
}

void IconManager::loadRecurently(QString dirPath, const QString& prefix, bool movie)
{
    QStringList extensions = movie ? movieFileExtensions : iconFileExtensions;
    QString path;
    QString name;
    QDir dir(dirPath);
    foreach (QFileInfo entry, dir.entryInfoList(extensions, QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot|QDir::Readable))
    {
        if (entry.isDir())
        {
            loadRecurently(entry.absoluteFilePath(), prefix+entry.fileName()+"_", movie);
            continue;
        }

        path = entry.absoluteFilePath();
        name = entry.baseName();
        paths[name] = path;
        if (movie)
            movies[name] = new QMovie(path);
        else
            icons[name] = new QIcon(path);

        if (path.startsWith(":/"))
        {
            if (movie)
                resourceMovies << name;
            else
                resourceIcons << name;
        }
    }
}

QMovie* IconManager::getMovie(const QString& name)
{
    if (!movies.contains(name))
        qCritical() << "Movie missing:" << name;

    return movies[name];
}

QIcon* IconManager::getIcon(const QString& name)
{
    if (!icons.contains(name))
        qCritical() << "Icon missing:" << name;

    return icons[name];
}

bool IconManager::isMovie(const QString& name)
{
    return movies.contains(name);
}
