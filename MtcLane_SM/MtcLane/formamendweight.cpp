#include "formamendweight.h"
#include "ui_formamendweight.h"
#include <QRect>
#include <QCoreApplication>
#include "MtcKey/MtcLaneState.h"
#include "statemachinetransition.h"
#include "formwtinfo.h"
#include "formmainwindow.h"
#include "FareCalc/FareCalcFunc.h"
#include "CommonModule/ilogmsg.h"
#include "formordinartprocess.h"
#include <QTableWidgetItem>
#include <QTabWidget>
#include "formauth.h"
#include "devicefactory.h"
#include "WtArm/wtdevunit.h"
#include "WtArm/wtdatadefineunit.h"
#include "WtArm/wtdatamgrunit.h"
bool flag1=false;
bool flag2=false;
Formamendweight::Formamendweight(QWidget *parent,QLayout *layout) :
    MtcOpWidget(parent,layout),
    ui(new Ui::Formamendweight), m_editWt(this),
    m_nEditLeft(0), m_nEditTop(0)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    m_editWt.hide();
    table=ui->tableWidget;
    table->setColumnCount(7);
    table->setRowCount(1);
    QStringList header;
    header<<"车序号"<<"轴数"<<"限载(t)"<<"总重(t)"<<"超限率(%)"<< "速度(km/h)"<<"状态";
    table->setHorizontalHeaderLabels(header);
    //    ui->tableWidget->setFocus();
    filterChildrenKeyEvent();
    table->removeRow(0);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

}

Formamendweight::~Formamendweight()
{
    //QDebug() << "release ....... " << objectName();
    delete ui;
}
//插入
void Formamendweight::on_pushButton_clicked()
{
    flag1=true;
    //    //调出输入总重车轴框
    connect(&m_editWt, SIGNAL(getWeigetAndAxls(int,int)), this, SLOT(DataEntered(int,int)));
    //计算编辑框的位置
    CalcLeftTop();
    //显示编辑框
    ShowEditWt();

}

void Formamendweight::on_pushButton_2_clicked()//修改
{
    flag2 =true;
    if(table->currentRow()<0)
    {
        ::showWarning(tr("无计重信息,不能进行此操作!"),true);
        return;
    }
    connect(&m_editWt,SIGNAL(getAmendWeigetAxls(int,int)),this,SLOT(DataAmend(int,int)));
    CalcLeftTop();
    ShowEditWt();
}

void Formamendweight::on_pushButton_3_clicked()//删除
{

    int row= table->currentRow();
    setindex(row);
    FormWtInfo *Pwt=NULL;
    getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
    if(row>=0)
    {
        table->removeRow(row);
        Pwt->DeleteAllInexInfo(row);
    }
    //修改序号
    showHHeadNum();
    //同步
    Pwt->showFormWtInfohead();
}
//TODO
//拆分
void Formamendweight::on_pushButton_4_clicked()
{
    if(table->currentRow()<0)
    {
        ::showWarning(tr("无计重信息,不能进行此操作!"),true);
        return;
    }

}
void Formamendweight::on_pushButton_6_clicked()
{
    FormWtInfo *Pwt=NULL;
    getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
    int tem=table->rowCount();
    for(int i=tem;i>=0;i--)
    {
        table->removeRow(i);
    }
    Pwt->ClearAll();
    QStringList header;
    header<<"车序号"<<"轴数"<<"限载(t)"<<"总重(t)"<<"超限率(%)"<< "速度(km/h)"<<"状态";
    table->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}


void Formamendweight::ShowEditWt()
{
    m_editWt.raise();
    m_editWt.move(m_nEditLeft, m_nEditTop);
    m_editWt.show();
    m_editWt.setFocus();
}

void Formamendweight::CalcLeftTop()
{
    m_nEditLeft = this->rect().right() - m_editWt.width()-10;
    m_nEditTop = this->rect().bottom() - m_editWt.height()-10;
    //QDebug() << m_nEditLeft << m_nEditTop;
}

void Formamendweight::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(!m_editWt.isHidden())
    {
        m_editWt.handleEvent(mtcKeyEvent);
        return;
    }
    if(mtcKeyEvent->isNumKey())
    {

        if(mtcKeyEvent->func()==Key1)
        {
            on_pushButton_clicked();
            mtcKeyEvent->setKeyType(KC_Number);
        }
        if(mtcKeyEvent->func()==Key2)
        {
            on_pushButton_2_clicked();
            mtcKeyEvent->setKeyType(KC_Number);
        }
        if(mtcKeyEvent->func() == Key3)
        {
            if(table->currentRow()<0)
            {
                ::showWarning(tr("无计重信息,不能进行此操作!"),true);
                return;
            }
            this->hide();
            getOrdinaryWidget()->hide();
            getMsgWidget()->setTitle(tr("编辑计重信息"));
            getMsgWidget()->setMessage(tr("是否要删除该车辆信息"));
            ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
            int nRlt = getMsgWidget()->doModalShow();
            if(nRlt)
            {
                this->show();
                this->setFocus();
                getOrdinaryWidget()->show();
                //                getOrdinaryWidget()->setFocus();
                on_pushButton_3_clicked();
                mtcKeyEvent->setKeyType(KC_Number);
            }
            else
            {
                this->show();
                this->setFocus();
                getOrdinaryWidget()->show();
                //                getOrdinaryWidget()->setFocus();
            }

        }
        if(mtcKeyEvent->func()==Key6)
        {
            if(table->currentRow()<0)
            {
                ::showWarning(tr("无计重信息,不能进行此操作!"),true);
                return;
            }
            this->hide();
            getOrdinaryWidget()->hide();
            getMsgWidget()->setTitle(tr("编辑计重信息"));
            getMsgWidget()->setMessage(tr("是否要清空所有车辆信息"));
            ::showWarning(tr("【确认】键继续，【取消】键返回"),true);
            int nRlt = getMsgWidget()->doModalShow();
            if(nRlt)
            {
                this->show();
                this->setFocus();
                getOrdinaryWidget()->show();
                on_pushButton_6_clicked();
                mtcKeyEvent->setKeyType(KC_Number);
                FormWtInfo *Pwt=NULL;
                getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
                Pwt->ClearAll();
            }
            else
            {
                this->show();
                this->setFocus();
                getOrdinaryWidget()->show();
            }
        }
    }

    if(mtcKeyEvent->isFuncKey())
    {
        handleUpAndDown(mtcKeyEvent);
        if(mtcKeyEvent->func() == KeyConfirm)
        {
            setResult(1);
        }
        if(mtcKeyEvent->func() ==KeyEsc)
        {
            setResult(0);
        }
    }
}
//计算超限率
int Formamendweight::OverWeightRate(int nTotalWeight,int nAxisNum)
{
    int nWeightLimit;
    int nOverweightRate;
    const int nMaxOverweightRate=9999;
    // NOTE: 编辑计重信息时手工输入限载，此时计算费率的限载不应根据轴数查找
    nWeightLimit=getWeightLimit(nAxisNum)/1000;

    if (nTotalWeight > nWeightLimit)
    {
        quint32 nOverweight;
        nOverweight=nTotalWeight-nWeightLimit;
        nOverweightRate=round_int32((double)1000*nOverweight/nWeightLimit);
    }
    else
    {
        nOverweightRate=0;
    }
    if (nOverweightRate>nMaxOverweightRate)
    {
        nOverweightRate=nMaxOverweightRate;
    }
    LogMsg("Weight",tr("超限率：nOverweightRate=1%,nOverweight=%2").arg(nOverweightRate).arg(nOverweightRate));
    return nOverweightRate;
}
void Formamendweight::DataEntered(int nWeight, int nAxls)
{
    //TODO:计算超重率/
    //表格显示数据

    if(flag1==true)
    {
        if(table->rowCount()<=0 || table->currentRow()<=0)
        {
            ui->tableWidget->insertRow(0);
            ui->tableWidget->setItem(0, 6, new QTableWidgetItem("输入"));
            ui->tableWidget->setItem(0, 5, new QTableWidgetItem(QString::number(0)));
            ui->tableWidget->setItem(0, 4, new QTableWidgetItem(QString::number(OverWeightRate(nWeight,nAxls)/10.0)));
            ui->tableWidget->setItem(0, 3, new QTableWidgetItem(QString::number(nWeight)));
            ui->tableWidget->setItem(0, 2, new QTableWidgetItem(QString::number(getWeightLimit(nAxls)/1000)));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(nAxls)));
        }
        else
        {
            int tmp = table->currentRow();
            ui->tableWidget->insertRow(tmp);
            ui->tableWidget->setItem(tmp, 6, new QTableWidgetItem("输入"));
            ui->tableWidget->setItem(tmp, 5, new QTableWidgetItem(QString::number(0)));
            ui->tableWidget->setItem(tmp, 4, new QTableWidgetItem(QString::number(OverWeightRate(nWeight,nAxls)/10.0)));
            ui->tableWidget->setItem(tmp, 3, new QTableWidgetItem(QString::number(nWeight)));
            ui->tableWidget->setItem(tmp, 2, new QTableWidgetItem(QString::number(getWeightLimit(nAxls)/1000)));
            ui->tableWidget->setItem(tmp, 1, new QTableWidgetItem(QString::number(nAxls)));
        }
        setCurrentRow(table->currentRow());
        FormWtInfo *Pwt=NULL;
        getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
        //轴数 限载 总重 超限率
        Pwt->inexitFormWtInfohead(nAxls,getWeightLimit(nAxls)/1000,nWeight,OverWeightRate(nWeight,nAxls));
        Pwt->inexitFormWtInfo(nAxls,getWeightLimit(nAxls)/1000,nWeight,OverWeightRate(nWeight,nAxls),0,tr("输入"));
        showHHeadNum();
        flag1=false;
    }
    else if(flag2 == false && flag1 == false)
    {
        TWtVehicle veh;
        TWtDataMgr vehMgr;
        int nRlt = table->currentRow();
        ui->tableWidget->insertRow(nRlt);
        ui->tableWidget->setItem(nRlt, 6, new QTableWidgetItem("称重"));
        ui->tableWidget->setItem(nRlt, 5, new QTableWidgetItem(QString::number(veh.getSpeed())));
        ui->tableWidget->setItem(nRlt, 4, new QTableWidgetItem(QString::number(veh.GetOverloadRate()/10.0)));
        ui->tableWidget->setItem(nRlt, 3, new QTableWidgetItem(QString::number(veh.GetWeight())));
        ui->tableWidget->setItem(nRlt, 2, new QTableWidgetItem(QString::number(veh.GetLimitWeight()/1000)));
        ui->tableWidget->setItem(nRlt, 1, new QTableWidgetItem(QString::number(vehMgr.GetVehicleCount())));
        setCurrentRow(table->currentRow());
        FormWtInfo *Pwt=NULL;
        getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
        //轴数 限载 总重 超限率
//        Pwt->inexitFormWtInfohead(nAxls,getWeightLimit(nAxls)/1000,nWeight,OverWeightRate(nWeight,nAxls));
//        Pwt->inexitFormWtInfo(nAxls,getWeightLimit(nAxls)/1000,nWeight,OverWeightRate(nWeight,nAxls),0,tr("输入"));
        showHHeadNum();
    }
//    TWtVehicle veh;
}

void Formamendweight::showHHeadNum()
{   FormWtInfo *Pwt=NULL;
    getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
        Pwt->shownumber(i);
    }
}

void Formamendweight::handleUpAndDown(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        if(mtcKeyEvent->func() == KeyDown)
        {
            if(ui->tableWidget->currentRow() >= 0)
            {
                ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow() + 1,0);
            }
            else
            {
                ui->tableWidget->setCurrentCell(0,0);
            }
        }
        else if(mtcKeyEvent->func() == KeyUp)
        {
            if(ui->tableWidget->currentRow() >= 1)
            {
                ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow() - 1, 0);
            }
            else
            {
                ui->tableWidget->setCurrentCell(0,0);
            }
        }
    }
}

void Formamendweight::DataAmend(int nWeight, int nAxls)
{
    if(flag2 == true)
    {
        int tmp=ui->tableWidget->currentRow();
        ui->tableWidget->setItem(tmp,1,new QTableWidgetItem(QString::number(nAxls)));
        ui->tableWidget->setItem(tmp,2,new QTableWidgetItem(QString::number(getWeightLimit(nAxls)/1000)));
        ui->tableWidget->setItem(tmp,3,new QTableWidgetItem(QString::number(nWeight)));
        ui->tableWidget->setItem(tmp,4,new QTableWidgetItem(QString::number(OverWeightRate(nWeight,nAxls)/10.0)));
        ui->tableWidget->setItem(tmp,5,new QTableWidgetItem(QString::number(0)));
        ui->tableWidget->setItem(tmp,6,new QTableWidgetItem("输入"));
        FormWtInfo *Pwt=NULL;
        getWidgetByType(emFormWtInfo, (QWidget**)&Pwt);
        Pwt->AmendCarInfo(tmp,nAxls,nWeight);
        flag2 = false;
    }
}

void Formamendweight::setindex(int dex)
{
    index=dex;
}

int Formamendweight::Getindex()
{
    return index;
}

void Formamendweight::setflagindex(int index)
{
    flagindex=index;
}

int Formamendweight::Getflagindex()
{
    return flagindex;
}
//TODO_liubo
Formamendweight * getAmendweight(QWidget *widget, QLayout *layout)
{
    static Formamendweight* g_pAmendweight = NULL;
    if(g_pAmendweight == NULL)
    {
        g_pAmendweight = new Formamendweight(widget,layout);
    }
    return g_pAmendweight;
}

void Formamendweight::setCurrentRow(int m_row)
{
    m_nCurrentRow = m_row;
}

int Formamendweight::getCurrentRow()
{
    return m_nCurrentRow;
}






