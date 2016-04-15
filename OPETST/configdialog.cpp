#include "configdialog.h"
#include "ui_configdialog.h"
#include "iconmanager.h"
#include "uiconfig.h"
#include "userinputfilter.h"
#include "mainwindow.h"
//#include "customconfigwidgetplugin.h"
//#include "config_builder/pluginmanager.h"
#include "config_builder/configmapper.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QComboBox>
#include <QStyleFactory>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QDesktopServices>
#include <QtUiTools/QUiLoader>
#include <QKeySequenceEdit>
#include <QPushButton>

#define GET_FILTER_STRING(Widget, WidgetType, Method) \
    if (qobject_cast<WidgetType*>(Widget))\
        return qobject_cast<WidgetType*>(Widget)->Method() + " " + Widget->toolTip();\

#define GET_FILTER_STRING2(Widget, WidgetType) \
    WidgetType* w##WidgetType = qobject_cast<WidgetType*>(widget);\
    if (w##WidgetType)\
        return getFilterString(w##WidgetType) + " " + Widget->toolTip();


ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    init();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::configureDataEditors(const QString& dataTypeString)
{
    ui->categoriesWidget->setVisible(false);
//    ui->stackedWidget->setCurrentWidget(ui->dataEditorsPage);

//    for (int i = 0; i < ui->dataEditorsTypesList->count(); i++)
//    {
//        if (ui->dataEditorsTypesList->item(i)->text() == dataTypeString.toUpper())
//        {
//            ui->dataEditorsTypesList->setCurrentRow(i);
//            return;
//        }
//    }

//    addDataType(dataTypeString.toUpper());
}

QString ConfigDialog::getFilterString(QWidget *widget)
{
    // Common code for widgets with single method call
    GET_FILTER_STRING(widget, QLabel, text);
    GET_FILTER_STRING(widget, QAbstractButton, text);
    GET_FILTER_STRING(widget, QLineEdit, text);
    GET_FILTER_STRING(widget, QTextEdit, toPlainText);
    GET_FILTER_STRING(widget, QPlainTextEdit, toPlainText);
    GET_FILTER_STRING(widget, QGroupBox, title);
    GET_FILTER_STRING(widget, QKeySequenceEdit, keySequence().toString);

    // Widgets needs a little more than single method call
    GET_FILTER_STRING2(widget, QComboBox);
    GET_FILTER_STRING2(widget, QTreeWidget);
    GET_FILTER_STRING2(widget, QListWidget);
    GET_FILTER_STRING2(widget, QTableWidget);

    return QString::null;
}

QString ConfigDialog::getFilterString(QComboBox *widget)
{
    QStringList items;
    for (int i = 0; i < widget->count(); i++)
        items << widget->itemText(i);

    return items.join(" ");
}

QString ConfigDialog::getFilterString(QTreeWidget *widget)
{
    QList<QTreeWidgetItem*> items = widget->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive);
    QStringList strList;
    foreach (QTreeWidgetItem* item, items)
        for (int i = 0; i < widget->columnCount(); i++)
            strList << item->text(i) + " " + item->toolTip(0);

    return strList.join(" ");
}

QString ConfigDialog::getFilterString(QListWidget *widget)
{
    QList<QListWidgetItem*> items = widget->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive);
    QStringList strList;
    foreach (QListWidgetItem* item, items)
        strList << item->text() + " " + item->toolTip();

    return strList.join(" ");
}

QString ConfigDialog::getFilterString(QTableWidget *widget)
{
    QList<QTableWidgetItem*> items = widget->findItems("*", Qt::MatchWildcard|Qt::MatchRecursive);
    QStringList strList;
    foreach (QTableWidgetItem* item, items)
         strList << item->text() + " " + item->toolTip();

    return strList.join(" ");
}

void ConfigDialog::init()
{
    ui->setupUi(this);
    setWindowIcon(ICONS.CONFIGURE);

    configMapper = new ConfigMapper(CfgMain::getPersistableInstances());
    connect(configMapper, SIGNAL(modified()), this, SLOT(markModified()));

    initPageMap();

    connect(ui->categoriesTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(switchPage(QTreeWidgetItem*)));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->activeStyleCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(updateStylePreview()));

    ui->activeStyleCombo->addItems(QStyleFactory::keys());

    load();
}

void ConfigDialog::load()
{
    updatingDataEditorItem = true;
    configMapper->loadToWidget(ui->stackedWidget);
    updatingDataEditorItem = false;
    setModified(false);
}


void ConfigDialog::initPageMap()
{
    int pages = ui->stackedWidget->count();
    QWidget* widget = nullptr;
    for (int i = 0; i < pages; i++)
    {
        widget = ui->stackedWidget->widget(i);
        nameToPage[widget->objectName()] = widget;
    }
}

void ConfigDialog::switchPage(QTreeWidgetItem *item)
{
    QString name = item->statusTip(0);
    if (!nameToPage.contains(name))
    {
        qWarning() << "Switched page to item" << name << "but there's no such named page defined in ConfigDialog.";
        return;
    }

    ui->stackedWidget->setCurrentWidget(nameToPage[name]);
}

void ConfigDialog::apply()
{
    //if (modifiedFlag)
        save();

    setModified(false);
}

void ConfigDialog::accept()
{
    apply();
    QDialog::accept();
}

void ConfigDialog::markModified()
{
    setModified(true);
}

void ConfigDialog::setModified(bool modified)
{
    modifiedFlag = modified;
    updateModified();
}

void ConfigDialog::updateModified()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(modifiedFlag);
}

void ConfigDialog::save()
{
    if (MainWindow::getInstance()->currentStyle().compare(ui->activeStyleCombo->currentText(), Qt::CaseInsensitive) != 0)
        MainWindow::getInstance()->setStyle(ui->activeStyleCombo->currentText());

    configMapper->saveFromWidget(ui->stackedWidget, true);

    if (requiresSchemasRefresh)
    {
        requiresSchemasRefresh = false;
        //DBTREE->refreshSchemas();
    }
    //MainWindow::getInstance()->updateCornerDocking();
}


void ConfigDialog::updateStylePreview()
{
    ui->previewWidget->parentWidget()->layout()->removeWidget(ui->previewWidget);
    ui->previewTabs->currentWidget()->layout()->addWidget(ui->previewWidget);
    ui->previewWidget->setEnabled(ui->previewTabs->currentIndex() == 0);

    QStyle* previousStyle = previewStyle;
    previewStyle = QStyleFactory::create(ui->activeStyleCombo->currentText());
    if (!previewStyle)
    {
        qWarning() << "Could not create style:" << ui->activeStyleCombo->currentText();
        return;
    }

    applyStyle(ui->activeStylePreviewGroup, previewStyle);

    if (previousStyle)
        delete previousStyle;
}

void ConfigDialog::applyStyle(QWidget *widget, QStyle *style)
{
    widget->setStyle(style);
    foreach (QObject* child, widget->children())
    {
        if (!qobject_cast<QWidget*>(child))
            continue;

        applyStyle(qobject_cast<QWidget*>(child), style);
    }
}
