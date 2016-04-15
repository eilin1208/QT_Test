#ifndef FORMVPR_H
#define FORMVPR_H

#include <QWidget>
#include "devicefactory.h"
#include "LogModule/qfilelogger.h"
namespace Ui {
    class FormVpr;
}

class FormVpr : public QWidget
{
    Q_OBJECT
signals:
    void onGetVpr(int nIndex);
public slots:
    void insertLog(const QString& sLog);
public:
    explicit FormVpr(QWidget *parent = 0);
    virtual ~FormVpr();
    void setCapImage(const QString& sFile);
    void clearImage();
    void changePage();
    void initPage();
    //设置日志接口
    void setLogger(QFileLogger* logger);
protected:
    void customEvent(QEvent *event);
private:
    bool SetVehPlatePic(const CVPRResult* pRlt);
private:
    Ui::FormVpr *ui;
};

#endif // FORMVPR_H
