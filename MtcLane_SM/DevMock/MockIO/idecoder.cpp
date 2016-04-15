#include "idecoder.h"
#include "formmcokfd.h"
QMap<int, IDecoder*> m_map;

IDecoder::IDecoder(QObject *parent) :
    QObject(parent)
{
}

IDecoder* getDecoder(int type)
{
    return m_map.value(type, NULL);
}

void addDecoder(int type, IDecoder *decoder)
{
    m_map.insert(type, decoder);
}
