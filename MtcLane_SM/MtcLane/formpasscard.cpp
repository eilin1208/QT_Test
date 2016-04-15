#include "formpasscard.h"
#include "ui_formpasscard.h"

FormPassCard::FormPassCard(QWidget *parent, QLayout* layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormPassCard)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
}

FormPassCard::~FormPassCard()
{
    delete ui;
}

void FormPassCard::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        mtcKeyEvent->setKeyType(KC_Func);
        if(mtcKeyEvent->func() == KeyEsc)
        {
            handleEscKey();
        }
        if(mtcKeyEvent->func() == KeyOffcial)
        {
            handleOfficalKey();
        }
    }
}

void FormPassCard::handleEscKey()
{
    setResult(0);
}

void FormPassCard::handleOfficalKey()
{
    setResult(1);
}

FormPassCard* getPassCardWgt(QWidget *widget, QLayout *layout)
{
    static FormPassCard* g_pPassCard = NULL;
    if(g_pPassCard == NULL)
    {
        g_pPassCard = new FormPassCard(widget, layout);
    }
    return g_pPassCard;
}
