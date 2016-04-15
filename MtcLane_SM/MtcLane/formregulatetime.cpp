#include "formregulatetime.h"
#include "ui_formregulatetime.h"
#include "CommonModule/laneutils_new.h"
#include "formmainwindow.h"
#include <QDate>
FormRegulateTime::FormRegulateTime(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormRegulateTime)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

FormRegulateTime::~FormRegulateTime()
{
    delete ui;
}

void FormRegulateTime::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->func() == KeyConfirm)
    {

        QFileInfo fileInfo1(qApp->applicationDirPath() + "/RunTime.ini");

        QSettings settings(fileInfo1.fileName() , QSettings::IniFormat);

        quint64 time1 = (settings.value("/runTime").toULongLong());
        quint64 time2 = ui->lineEdit->text().toULongLong();
        QDateTime time = QDateTime::fromString(ui->lineEdit->text(), "yyyyMMddhhmmss");
        if(time.isValid() && time1 <= time2/*&& time1 >= time*//*time.date().year() >= 2013*/)
        {
            setSystemDateTime(time);
            setResult(1);
        }
        else
        {
            showWarning(tr("输入时间格式错误，请输入正确的时间"));
        }
    }
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
    }
    if(mtcKeyEvent->func() == KeyDel)
    {
        ui->lineEdit->setText(ui->lineEdit->text().left(ui->lineEdit->text().length() - 1));
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

FormRegulateTime *getRegulateTimeForm(QWidget *widget, QLayout *layout)
{
    static FormRegulateTime* g_pFunc = NULL;
    if(g_pFunc == NULL)
    {
        g_pFunc = new FormRegulateTime(widget, layout);
    }
    return g_pFunc;
}
