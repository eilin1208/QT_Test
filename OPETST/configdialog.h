#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "config_builder.h"
#include "common/bihash.h"

class QListWidgetItem;
class QTreeWidgetItem;
class QSignalMapper;
class QComboBox;
class QToolButton;
class QTreeWidget;
class QListWidget;
class QTableWidget;
class QPushButton;
class ConfigMapper;

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

    void configureDataEditors(const QString& dataTypeString);

    static QString getFilterString(QWidget* widget);
    static QString getFilterString(QComboBox* widget);
    static QString getFilterString(QTreeWidget* widget);
    static QString getFilterString(QListWidget* widget);
    static QString getFilterString(QTableWidget* widget);

private:
    void init();
    void load();
    void initPageMap();
    void applyStyle(QWidget* widget, QStyle* style);

    BiHash<QTreeWidgetItem*,QString> itemToPluginNameMap;
    ConfigMapper* configMapper = nullptr;
    bool requiresSchemasRefresh = false;
    bool updatingDataEditorItem = false;
    QStyle* previewStyle = nullptr;

private slots:
    void updateStylePreview();
    void markModified();
    void setModified(bool modified);
    void updateModified();
    void switchPage(QTreeWidgetItem* item);
    void apply();
    void save();

public slots:
    void accept();


private:
    Ui::ConfigDialog *ui;

    bool modifiedFlag = false;
    QHash<QString,QWidget*> nameToPage;
};

#endif // CONFIGDIALOG_H
