#ifndef CUSTOMCONFIGWIDGETPLUGIN_H
#define CUSTOMCONFIGWIDGETPLUGIN_H

#include "config_builder/plugins/plugin.h"

#include <QVariant>

class CfgEntry;
class QWidget;

class CustomConfigWidgetPlugin : public virtual Plugin
{
    public:
        virtual bool isConfigForWidget(CfgEntry* key, QWidget* widget) = 0;
        virtual void applyConfigToWidget(CfgEntry* key, QWidget* widget, const QVariant &value) = 0;
        virtual QVariant getWidgetConfigValue(QWidget* widget, bool& ok) = 0;
        virtual const char* getModifiedNotifier() const = 0;
        virtual QString getFilterString(QWidget* widget) const = 0;
};

#endif // CUSTOMCONFIGWIDGETPLUGIN_H
