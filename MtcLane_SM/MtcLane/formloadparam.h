#ifndef FORMLOADPARAM_H
#define FORMLOADPARAM_H

#include <QWidget>
#include "MtcKey/MtcOpWidget.h"
#include "reqparam.h"
namespace Ui {
    class FormLoadParam;
}

class FormLoadParam : public MtcOpWidget
{
    Q_OBJECT
public:
    FormLoadParam(QWidget *parent=0, QLayout *layout=0);
    virtual ~FormLoadParam();
public slots:
    void ShowFileInfo(int nFile);
    void ShowDownloading(quint32 cfgFile);
    void ShowPreDownload(quint32 cfgFile, bool bNew, bool bRet);
    void ShowDownloaded(quint32 cfgFile, bool bNew, bool bRet);
public:
    void ReLoadAll();
protected:
    void showEvent(QShowEvent *event);
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
private:
    Ui::FormLoadParam *ui;
    QMap<int, int> m_map;
};

FormLoadParam* getLoadParam(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMLOADPARAM_H
