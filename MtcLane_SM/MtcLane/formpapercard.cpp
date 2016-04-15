#include "formpapercard.h"
#include "ui_formpapercard.h"
#include "ParamModule/qorgcodeparamfile.h"
#include "ParamModule/qparammgr.h"
#include "formmainwindow.h"
#include "laneinfo.h"
FormPaperCard::FormPaperCard(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormPaperCard)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

FormPaperCard::~FormPaperCard()
{
    delete ui;
}

QString FormPaperCard::getEnterNo()
{
    return ui->lineEdit->text();
}

void FormPaperCard::verifyInputNum()
{
    if(ui->lineEdit->text().length() < 4)
    {
        ::showWarning(tr("入口站编号不能小于4位"));
        return;
    }
    quint32 value1=ui->lineEdit->text().toUInt();
    QOrgCodeParamFile* orgCode = (QOrgCodeParamFile*)getParamMgr()->GetParamFile(cfOrgCode);
    SOrgCodeValue value2;
    bool ret=orgCode->queryOrgCodeValue(value2,value1,ORG_TYPE_STATION);
    if(ret)
    {
        //getLaneInfo()->setOrgCode(ui->lineEdit->text());
        emit entryStationRight();
    }
    else
    {
        emit entryStationWrong();
        ::showWarning("入口站编号错误");
    }


}

void FormPaperCard::showEvent(QShowEvent *event)
{
    ui->lineEdit->setFocus();
    ui->lineEdit->clear();
}

void FormPaperCard::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
        return;
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyDel)
    {
        ui->lineEdit->backspace();
    }
    if(mtcKeyEvent->isFuncKey()&&((mtcKeyEvent->func()==KeyUp)||(mtcKeyEvent->func()==KeyDown)||(mtcKeyEvent->func()==KeyLeft)||(mtcKeyEvent->func()==KeyRight)))
    {
        ui->lineEdit->clear();
    }
    if(mtcKeyEvent->isFuncKey()&& mtcKeyEvent->func()==KeyConfirm)
    {
        verifyInputNum();
    }
    if(mtcKeyEvent->func() != KeyEsc)
    {
        mtcKeyEvent->setKeyType(KC_Func);
    }
}

