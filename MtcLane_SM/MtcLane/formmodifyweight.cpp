#include "formmodifyweight.h"
#include "ui_formmodifyweight.h"
#include "WtArm/wtdatamgrunit.h"
#include "formmainwindow.h"
#include "formmessage.h"
#include "lanectrl.h"
#include "exittollitem.h"
#include "formbase.h"
FormModifyWeight::FormModifyWeight(QWidget *parent, QLayout* layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormModifyWeight), m_nStatus(0)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    connect(getWtDataMgr(), SIGNAL(WtDataStatusChanged(int,int)),
            this, SLOT(WtDataChanged(int,int)));
//    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);


    ui->tableWidget->setShowGrid(true);
}

FormModifyWeight::~FormModifyWeight()
{
    delete ui;
}

void FormModifyWeight::WtDataChanged(int nEvent, int nId)
{
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    int i =0;
    //qDebug() << "id is " << nId;
    switch(nEvent)
    {
    case dccNewVeh:
        //在底部插入一行
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccInsertVeh:
        ui->tableWidget->insertRow(nId - 1);
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccSpitVeh:
        ui->tableWidget->insertRow(nId);
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccModifyVeh:
        ui->tableWidget->selectRow(nIndex);
        changeData();
        break;
    case dccBackVeh:
    case dccDeleteVeh:
    case dccMergeVeh:
        for(i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if(ui->tableWidget->item(i, 0)->text().toInt() == nId)
            {
                ui->tableWidget->removeRow(i);
                break;
            }
        }
        changeData();
        break;
    default:
        break;

    }
}

void FormModifyWeight::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{

    switch(m_nEditStatus)
    {
    case esInit:
        chooseFunc(mtcKeyEvent);
        break;
    case esWeight:
        inputWeight(mtcKeyEvent);
        break;
    case esAxisNo:
        inputAxisNo(mtcKeyEvent);
        break;
    case esAxisCount:
        inputAxisCount(mtcKeyEvent);
        break;
    default:
        break;
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

void FormModifyWeight::chooseFunc(MtcKeyPressedEvent *mtcKeyEvent)
{
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex < 0)
    {
        nIndex = 0;
    }
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);

        int keyNum = mtcKeyEvent->getLogicKeyName().toInt();
        switch(keyNum)
        {
        case 1:
            addNew();
            break;
        case 2:
            modifyOne();
            break;
        case 3:
            deleteOne();
            break;
        case 4:
            split();
            break;
        case 5:
            combine();
            break;
        case 6:
            clearAll();
            break;
        default:
            break;
        }
    }
    if(mtcKeyEvent->isFuncKey())
    {
        switch(mtcKeyEvent->func())
        {
        case KeyConfirm:
            setResult(1);
            break;
        case KeyEsc:
            setResult(0);
            break;
        case KeyUp:
            if(nIndex >= 0)
            {
                ui->tableWidget->selectRow(ui->tableWidget->currentRow() - 1);
            }
            break;
        case KeyDown:
            if(nIndex >= 0)
            {
                ui->tableWidget->selectRow(ui->tableWidget->currentRow() + 1);
            }
            break;
        default:
            break;
        }
    }
}

void FormModifyWeight::showEvent(QShowEvent *)
{
    setEditStatus(esInit);
    m_nStatus = 0;
    m_bModify = false;
    if(ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->selectRow(0);
    }
}

void FormModifyWeight::addNew()
{
    m_nStatus = 1;
    setEditStatus(esWeight);
}

void FormModifyWeight::deleteOne()
{
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    if(getWtDataMgr()->GetVehicleCount() == 0)
    {
        ::showWarning(tr("无计重信息，不能进行修改操作！"));
        return;
    }
    m_nStatus = 3;
    //确认合并
    getOrdinaryWidget()->hide();
    getMsgWidget()->setTitle(tr("编辑计重信息"));
    getMsgWidget()->setMessage(tr("是否要删除该车辆信息？"));
    ::showWarning(tr("请【确认】键继续，【取消】键返回"));
    int ret = getMsgWidget()->doModalShow();
    if(ret == 1 && nIndex > -1)
    {
        getWtDataMgr()->Delete(nIndex + 1);
        ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
        pExit->SendModWeightInfo(MWT_DelInfo);
    }
    getOrdinaryWidget()->show();
    this->setFocus();
}

void FormModifyWeight::modifyOne()
{
    if(getWtDataMgr()->GetVehicleCount() == 0)
    {
        ::showWarning(tr("无计重信息，不能进行修改操作！"));
        return;
    }
    m_nStatus = 2;
    m_bModify = true;
    setEditStatus(esWeight);
}

void FormModifyWeight::split()
{
    int nIndex = ui->tableWidget->currentRow();
    if(getWtDataMgr()->GetVehicleCount() == 0)
    {
        ::showWarning(tr("无计重信息，不能进行拆分操作！"));
        return;
    }
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    if(getWtDataMgr()->GetVehicle(nIndex + 1)->GetAxisCount() < 2)
    {
        ::showWarning(tr("轴数小于2，无法拆分！"));
        return;
    }
    if(getWtDataMgr()->GetVehicle(nIndex + 1)->getDataStatus() == dsEdit)
    {
        ::showWarning(tr("输入的车辆，无法拆分！"));
        return;
    }
    m_nStatus = 4;
    setEditStatus(esAxisNo);
}

void FormModifyWeight::combine()
{
    int nIndex = ui->tableWidget->currentRow();
    if(getWtDataMgr()->GetVehicleCount() == 0)
    {
        ::showWarning(tr("无计重信息，不能进行合并操作！"));
        return;
    }
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    if(getWtDataMgr()->GetVehicleCount() < 2)
    {
        ::showWarning(tr("至少要有两辆车才可以合并！"));
        return;
    }
    if(getWtDataMgr()->GetVehicleCount() <= (nIndex + 1))
    {
        ::showWarning(tr("最后一辆车无法合并！"));
        return;
    }
    if(getWtDataMgr()->GetVehicle(nIndex + 1)->getDataStatus() == dsEdit &&
            getWtDataMgr()->GetVehicle(nIndex + 2)->getDataStatus() == dsEdit)
    {
        ::showWarning(tr("输入的车辆，无法合并！"));
        return;
    }
    m_nStatus = 5;
    //确认合并
    getOrdinaryWidget()->hide();
    getMsgWidget()->setTitle(tr("编辑计重信息"));
    getMsgWidget()->setMessage(tr("是否要合并车辆信息？"));
    ::showWarning(tr("请【确认】键继续，【取消】键返回"));
    int ret = getMsgWidget()->doModalShow();
    if(ret == 1)
    {
        if(nIndex != -1 && nIndex < ui->tableWidget->rowCount() -1)
        {
            getWtDataMgr()->Merge(nIndex + 1);
            ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
            pExit->SendModWeightInfo(MWT_MergeInfo);
        }
    }
    getOrdinaryWidget()->show();
    this->setFocus();
}

void FormModifyWeight::clearAll()
{
    if(getWtDataMgr()->GetVehicleCount() < 1)
    {
        ::showWarning(tr("无计重信息，不能进行清空操作！"));
        return;
    }
    m_nStatus = 6;
    //清空
    getOrdinaryWidget()->hide();
    getMsgWidget()->setTitle(tr("编辑计重信息"));
    getMsgWidget()->setMessage(tr("是否要清空所有计重信息？"));
    ::showWarning(tr("请【确认】键继续，【取消】键返回"));
    int ret = getMsgWidget()->doModalShow();
    if(ret == 1)
    {
        int vehCount = getWtDataMgr()->GetVehicleCount();
        for(int i = 0; i < vehCount; i++)
        {
            getWtDataMgr()->Delete(1);
            ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
            pExit->SendModWeightInfo(MWT_DelInfo);
        }
    }
    getOrdinaryWidget()->show();
    this->setFocus();
}

void FormModifyWeight::setEditStatus(TEditStatus status)
{
    m_nEditStatus = status;
    QString sTmp;
    switch(status)
    {
    case esInit:
        sTmp = tr("按【↑】【↓】键选择编辑的车辆");
        m_dTotalWieght = 0;
        m_nAxisCount = 0;
        m_nAxisNo = 0;
        m_bModify = false;
        m_nStatus = 0;
        ui->editWidget->hide();
        break;
    case esWeight:
        showInputInfo(tr("请输入总重\n(单位:吨)"));
        sTmp = tr("请输入总重，按【确定】键确认");
        break;
    case esAxisNo:
        showInputInfo(tr("请输入轴号"));
        sTmp = tr("请输入轴号，按【确定】键确认");
        break;
    case esAxisCount:
        showInputInfo(tr("请输入轴数"));
        sTmp = tr("请输入轴数，按【确定】键确认");
        break;
    default:
        break;
    }
    switch(m_nStatus)
    {
    case 1:
        sTmp = "【插入】" + sTmp;
        break;
    case 2:
        sTmp = "【修改】" + sTmp;
        break;
    case 4:
        sTmp = "【拆分】" + sTmp;
        break;
    default:
        break;
    }
    ::showWarning(sTmp);
}

void FormModifyWeight::showInputInfo(const QString &sInfo)
{
    ui->lineEdit->clear();
    ui->editWidget->show();
    ui->label->setText(sInfo);
}

void FormModifyWeight::inputWeight(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        switch(mtcKeyEvent->func())
        {
        case KeyPoint:
            if(!ui->lineEdit->text().contains('.'))
            {
                ui->lineEdit->setText(ui->lineEdit->text() + mtcKeyEvent->getLogicKeyName());
                m_dTotalWieght = getNumber(ui->lineEdit->text());
            }
            break;
        case KeyConfirm:
            if(m_dTotalWieght <= 0)
            {
                ::showWarning(tr("总重不能为空"));
                return;
            }
            if(m_dTotalWieght >999)
            {
                ::showWarning(tr("请输入小于999的数值"),true);
                ui->lineEdit->clear();
                return;
            }
            ui->editWidget->hide();
            setEditStatus(esAxisCount);
            break;
        case KeyDel:
            if(ui->lineEdit->text().length() > 0)
            {
                ui->lineEdit->setText(ui->lineEdit->text().remove(ui->lineEdit->text().length() -1, 1));
                m_dTotalWieght = getNumber(ui->lineEdit->text());
            }
            break;
        case KeyEsc:
            setEditStatus(esInit);
            break;
        }
        mtcKeyEvent->setKeyType(KC_Func);
    }

    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text() + mtcKeyEvent->getLogicKeyName());
        m_dTotalWieght = getNumber(ui->lineEdit->text());
    }
}

void FormModifyWeight::inputAxisNo(MtcKeyPressedEvent *mtcKeyEvent)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text() + mtcKeyEvent->getLogicKeyName());
        m_nAxisNo = getNumber(ui->lineEdit->text());
    }
    if(mtcKeyEvent->isFuncKey())
    {
        mtcKeyEvent->setKeyType(KC_Func);
        switch(mtcKeyEvent->func())
        {
        case KeyConfirm:
            if(m_nAxisNo <= 0)
            {
                ::showWarning(tr("轴号不能为空"));
                return;
            }
            //拆分操作
            if(m_nAxisNo > 0 &&
                    m_nAxisNo < ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->text().toInt())
            {
                getWtDataMgr()->Split(ui->tableWidget->currentRow() + 1, m_nAxisNo);
            }
            pExit->SendModWeightInfo(MWT_SplitInfo);
            ui->editWidget->hide();
            setEditStatus(esInit);
            break;
        case KeyDel:
            if(ui->lineEdit->text().length() > 0)
            {
                ui->lineEdit->setText(ui->lineEdit->text().remove(ui->lineEdit->text().length() -1, 1));
                m_nAxisNo = getNumber(ui->lineEdit->text());
            }
            break;
        case KeyEsc:
            setEditStatus(esInit);
            break;
        }
    }
}

void FormModifyWeight::inputAxisCount(MtcKeyPressedEvent *mtcKeyEvent)
{
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        ui->lineEdit->setText(ui->lineEdit->text() + mtcKeyEvent->getLogicKeyName());
        m_nAxisCount = getNumber(ui->lineEdit->text());
    }
    int nIndex = ui->tableWidget->currentRow();
    if(nIndex == -1)
    {
        nIndex = 0;
    }
    if(mtcKeyEvent->isFuncKey())
    {
        mtcKeyEvent->setKeyType(KC_Func);
        switch(mtcKeyEvent->func())
        {
        case KeyConfirm:
            if(m_nAxisCount <= 0)
            {
                ::showWarning(tr("轴数不能为空"));
                return;
            }
            if(m_nAxisCount > 50)
            {
                ::showWarning(tr("请输入轴数小于50的数值"),true);
                ui->lineEdit->clear();
                return;
            }
            if(!m_bModify)
            {
                getWtDataMgr()->Insert(nIndex + 1, qRound(1000* m_dTotalWieght), m_nAxisCount);
                pExit->SendModWeightInfo(MWT_InInfo);
            }
            else
            {
                getWtDataMgr()->Modify(nIndex + 1, qRound(1000* m_dTotalWieght), m_nAxisCount);
                pExit->SendModWeightInfo(MWT_ModInfo);
            }
            ui->editWidget->hide();
            setEditStatus(esInit);
            m_bModify = false;
            break;
        case KeyDel:
            if(ui->lineEdit->text().length() > 0)
            {
                ui->lineEdit->setText(ui->lineEdit->text().remove(ui->lineEdit->text().length() -1, 1));
                m_nAxisCount = getNumber(ui->lineEdit->text());
            }
            break;
        case KeyEsc:
            setEditStatus(esInit);
            break;
        }
    }
}

double FormModifyWeight::getNumber(QString sNum)
{
    if(sNum.right(1).compare(".") == 0)
    {
        sNum.remove(sNum.length() - 1, 1);
    }
    return sNum.toDouble();
}

void FormModifyWeight::changeData()
{
    TWtVehicle* wt = NULL;
    for(int i = 0; i < getWtDataMgr()->GetVehicleCount(); i++)
    {
        wt = getWtDataMgr()->GetVehicle(i + 1);
        if(wt != NULL)
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(wt->GetAxisCount())));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(wt->GetLimitWeight() / 1000.0,'f' ,2)));
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(wt->GetWeight() / 1000.0,'f' ,2)));
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(wt->GetOverloadRate() / 10.0)));
            ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(wt->getSpeed(), 'f', 2)));
            ui->tableWidget->setItem(i, 6, new QTableWidgetItem(wt->GetDataStatusName()));
        }
    }
}

FormModifyWeight* getModifyWeight(QWidget* widget, QLayout* layout)
{
    static FormModifyWeight* g_pModify = NULL;
    if(g_pModify == NULL)
    {
        g_pModify = new FormModifyWeight(widget, layout);
    }
    return g_pModify;
}
