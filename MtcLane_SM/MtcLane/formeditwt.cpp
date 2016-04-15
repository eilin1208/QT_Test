#include "formeditwt.h"
#include "ui_formeditwt.h"
#include "formmainwindow.h"
#include <QDebug>
#include "formwtinfo.h"
const char* sEditWt = "请输入总重";
const char* sNote = "(单位：吨)";
const char* sEditAxls = "请输入轴数";
FormEditWt::FormEditWt(QWidget *parent, QLayout* layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::FormEditWt), m_bStep(true), m_nWeidget(0), m_nAxlsNum(0)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

FormEditWt::~FormEditWt()
{
    //qDebug() << "release ....... " << objectName();
    delete ui;
}

void FormEditWt::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        //qDebug() << "get here";
        ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
    }
    if(mtcKeyEvent->isFuncKey()&&ui->lineEdit->text().isEmpty()==true)
    {
        if(m_bStep)
        {
            ::showWarning(tr("请输入总重"), true, FormWarning::WARNING);
        }
        else
        {
            ::showWarning(tr("请输入轴数"), true, FormWarning::WARNING);
        }
    }
    if(mtcKeyEvent->func()==KeyConfirm && ui->lineEdit->text().isEmpty()==false)
    {
        showText();
        m_bStep =!m_bStep;
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyDel)
    {
        ui->lineEdit->backspace();
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() ==KeyEsc)
    {
        this->hide();
        ui->lineEdit->clear();
        ::showWarning(tr(""), true, FormWarning::WARNING);
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyConfirm)
    {
        FormWtInfo *Pwt=NULL;
        getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
        //同步
        Pwt->showFormWtInfohead();

    }

    mtcKeyEvent->setKeyType(KC_Func);

}

void FormEditWt::showText()
{
    if(m_bStep)
    {
        ui->label->setText(QString(sEditAxls));
        ui->label_2->setText("");
        m_nWeidget = ui->lineEdit->text().toInt();
        ui->lineEdit->clear();
    }
    else
    {
        m_nAxlsNum = ui->lineEdit->text().toInt();
        ui->label->setText(QString(sEditWt));
        ui->label_2->setText(QString(sNote));
        ui->lineEdit->clear();
        this->hide();
        emit getWeigetAndAxls(m_nWeidget, m_nAxlsNum);
        emit getAmendWeigetAxls(m_nWeidget,m_nAxlsNum);
    }
}



void FormEditWt::handleEvent(MtcKeyPressedEvent *mtcKeyEvent)
{
    mtcKeyPressed(mtcKeyEvent);
}

void FormEditWt::showEvent(QShowEvent * event)
{
    m_bStep = true;
    ui->label->setText(sEditWt);
    ui->label_2->setText(sNote);
}
