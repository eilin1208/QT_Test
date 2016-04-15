#include "basesaver.h"

BaseSaver::BaseSaver(QObject *parent) :
    QObject(parent)
{
   FStaID = 0;
   FLaneID = 0;
   FHasTempData = false;
}



