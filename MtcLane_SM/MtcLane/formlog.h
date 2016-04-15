#ifndef FORMLOG_H
#define FORMLOG_H

#include <QWidget>
namespace Ui {
class FormLog;
}

class FormLog : public QWidget
{
    Q_OBJECT
signals:

public:
    explicit FormLog(QWidget *parent = 0);
    ~FormLog();
    /*!
     \brief
     显示日志
     \param sLog
     日志内容
    */
    void showLog(const QString& sLog);

public:
    void setVehCount();
    int getVehCount()
    {
        return vehCount;
    }

    void clearVehCount();

private:
    enum FUNCTYPE
    {
        NONE,
        MOTOR,
        MOTOREND,
        LOCKBAR,
        UNLOCKBAR
    };
private:
    Ui::FormLog *ui;
    enum
    {
        //最多显示行数
        MAXROW = 100
    };
    //清除多余行
    void clearReset();

private:
    FUNCTYPE m_funcType;
    int vehCount;
};

#endif // FORMLOG_H
