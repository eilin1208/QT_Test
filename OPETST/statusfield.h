#ifndef STATUSFIELD_H
#define STATUSFIELD_H

#include <QDockWidget>
#include <QVariant>
#include <QDockWidget>

class QMenu;
class QAbstractAnimation;
class QTableWidgetItem;

namespace Ui {
class StatusField;
}

class StatusField : public QDockWidget
{
    Q_OBJECT

public:
    explicit StatusField(QWidget *parent = 0);
    ~StatusField();
    bool hasMessages() const;

protected:
    void changeEvent(QEvent *e);

private:
    void addEntry(const QIcon& icon, const QString& text, const QColor &color);
    void flashItems(const QList<QTableWidgetItem*>& items, const QColor& color);
    void setupMenu();
    void readRecentMessages();

private slots:
    void customContextMenuRequested(const QPoint& pos);
    void info(const QString& text);
    void warn(const QString& text);
    void error(const QString& text);
    void reset();
    void fontChanged(const QVariant& variant);

signals:
    void linkActivated(const QString& link);

private:
    Ui::StatusField *ui;

    QMenu* menu = nullptr;
    QAction* copyAction = nullptr;
    QAction* clearAction = nullptr;
    QList<QAbstractAnimation*> itemAnimations;
    bool noFlashing = false;

    static const int timeStampColumnWidth = 70;
    static const int itemCountLimit = 30;
    static constexpr const char* timeStampFormat = "hh:mm:ss";
};

#endif // STATUSFIELD_H
