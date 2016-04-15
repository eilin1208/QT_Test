#ifndef XMLBAKUPWRITER_H
#define XMLBAKUPWRITER_H

#include <QObject>
#include <QFile>
#include <QXmlStreamWriter>
#include "CommonModule/MsgDefine.h"
class XMLBakupWriter : public QObject
{
    Q_OBJECT
public:
    explicit XMLBakupWriter(QObject *parent = 0);
    ~XMLBakupWriter();
signals:
    
public slots:
public:
    void bakup(TDataToSave& data);
private:
    bool checkFileOpen();
    bool checkFileSize();
    int getBakupSize();
    bool writeData();
private:
    QFile m_curFile;    //当前正在读写的配置文档
    QXmlStreamWriter m_xmlWriter;
};

inline XMLBakupWriter* getXMLBakupWriter()
{
    static XMLBakupWriter writer;
    return &writer;
}

#endif // XMLBAKUPWRITER_H
