#include "iniinfo.h"
const char* FILENAME = "./Lane.ini";
IniInfo::IniInfo(QObject *parent) :
    QObject(parent)
{
}

int IniInfo::getLaneType()
{
    return 0;
}

QVariant IniInfo::getValue(const QString &sKey)
{
    QSettings settings(FILENAME, QSettings::IniFormat);
    return settings.value(sKey);
}
