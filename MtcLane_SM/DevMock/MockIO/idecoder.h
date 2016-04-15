#ifndef IDECODER_H
#define IDECODER_H
#include "datadefine.h"
#include <QObject>
class IDecoder : public QObject
{
    Q_OBJECT
public:
    explicit IDecoder(QObject *parent = 0);

signals:
    void finishDecode();

public slots:
public:
    virtual void decode(QByteArray& data) = 0;
    virtual void getValue(int key, QVariant& value) = 0;
protected:
    QMap< int , QVariant > m_data;
};

IDecoder* getDecoder(int type);

void addDecoder(int type, IDecoder* decoder);

#endif // IDECODER_H
