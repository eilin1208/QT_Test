#include "fdinterface.h"

FDInterface* g_instance = NULL;

FDInterface::FDInterface(QObject *parent) :
    QThread(parent)
{
}

void setInstance(FDInterface *instance)
{
    g_instance = instance;
}

FDInterface* getInstance()
{
    return g_instance;
}
