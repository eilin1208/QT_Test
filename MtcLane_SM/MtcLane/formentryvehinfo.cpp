#include "formentryvehinfo.h"
#include "ui_formentryvehinfo.h"
#include "CommonModule/LaneType.h"
#include "laneinfo.h"
#include "devicefactory.h"
#include "formmainwindow.h"
#include "lanectrl.h"
#include "exittollitem.h"
#include "formvpr.h"
#include "cardinfoprocessor.h"
#include "entrycardinfoprocessor.h"
#include "formbase.h"
#include "formmessage.h"
QString getQssByPlateColor(int nColor);
FormEntryVehInfo::FormEntryVehInfo(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormEntryVehInfo)
{
    ui->setupUi(this);
    this->showResultMode(false);
    m_bResultMode = false;
    m_bFirstInput = false;
    FormVpr* pVpr = NULL;
    getWidgetByType(emFormVpr, (QWidget**)&pVpr);
    connect(pVpr, SIGNAL(onGetVpr(int)), this, SLOT(getVprResult(int)));
}

FormEntryVehInfo::~FormEntryVehInfo()
{
    delete ui;
}

void FormEntryVehInfo::showResultMode(bool bRes)
{
    ui->labEnVehType->setEnabled(!bRes);
    ui->labEnVehPlate->setEnabled(!bRes);
    ui->labEnMoney->setVisible(bRes);
    ui->labMoney->setVisible(bRes);
    ui->labEnMoney->setEnabled(bRes);
    ui->labMoney->setEnabled(bRes);
    m_bResultMode = bRes;
    if(bRes)
    {
        m_cardDetecter.Start();
    }
}


void FormEntryVehInfo::setPlateInfo(int nColor, const QString &sPlate)
{
    ui->labEnVehPlate->setStyleSheet(getQssByPlateColor(nColor));
    if(sPlate.length() == 0)
    {
        ui->labEnVehPlate->setText(tr("无车牌"));
    }
    else
    {
        ui->labEnVehPlate->setText(sPlate);
    }
}

bool FormEntryVehInfo::hasEnterVehType()
{
    return !ui->labEnVehType->text().isEmpty();
}

void FormEntryVehInfo::setTollFee(int nFee, int nCardFee)
{
    if(nCardFee != 0)
    {
        ui->labelCard->show();
        ui->labelCard->setText(tr("卡成本：%1元").arg(nCardFee));
    }
    ui->labEnMoney->setText(QString::number(nFee + nCardFee) + tr("元"));

}

void FormEntryVehInfo::setTollFreeFee(int nFree)
{
    if(getLaneInfo()->getVehType() != VT_Normal)
    {
        ui->label_3->setText(GetVehClassNoByName_LPCSTR((CVehType)getLaneInfo()->getVehType()));
        ui->label_2->setText("免费金额："+QString::number(nFree).append("元"));
        ui->label_2->show();
        ui->label_3->show();
    }
    else
    {
        ui->label_2->clear();
        ui->label_3->clear();
        ui->label_2->hide();
        ui->label_3->hide();
    }
}

bool FormEntryVehInfo::IsResultMode()
{
    return m_bResultMode;
}

bool FormEntryVehInfo::hasFinishVehInfo()
{
    return (!ui->labEnVehClass->text().isEmpty() &&
            !ui->labEnVehPlate->text().isEmpty() &&
            !ui->labEnVehType->text().isEmpty());
}

void FormEntryVehInfo::getVehPlate(int &nColor, QString &sPlate)
{
    nColor = m_nPlateColor;
    sPlate = ui->labEnVehPlate->text();
}

void FormEntryVehInfo::setVehClass(const QString &sVehClass, int nVehClass)
{
    m_nVehClass = nVehClass;
    ui->labEnVehClass->setText(sVehClass);
}

void FormEntryVehInfo::clearData()
{
    ui->labEnMoney->clear();
    ui->labEnVehClass->clear();
    ui->labEnVehPlate->clear();
    ui->labEnVehType->clear();
    ui->labEnVehPlate->setStyleSheet("");
    ui->label_2->clear();
    ui->label_2->hide();
    ui->label_3->clear();
    ui->label_3->hide();
    ui->labelCard->clear();
    ui->labelCard->hide();
}

void FormEntryVehInfo::stopCardStateDetecter()
{
    m_cardDetecter.Close();
}

void FormEntryVehInfo::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(!m_bResultMode)
    {
        //判断硬盘空间是否足够
        if(!isDiskSpaceEnough() && mtcKeyEvent->func() != KeyShift)
        {
            //如果硬盘空间不足，提示下班！
            getOrdinaryWidget()->hide();
            getMsgWidget()->setTitle(tr(""));
            getMsgWidget()->setMessage(tr("硬盘空间容量过低\n请立刻下班,并联系维护人员进行维护\n按【确认】键继续"));
            getMsgWidget()->doModalShow();
            getOrdinaryWidget()->show();
            this->setFocus();
            mtcKeyEvent->setKeyType(KC_Func);
            return;
        }
        if(mtcKeyEvent->isVehTypeKey())
        {
            handleVehTypeKey(mtcKeyEvent);
        }
        //
        if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyPoint)
        {
            ui->labEnVehPlate->setText(tr("无车牌"));
            mtcKeyEvent->setKeyType(KC_Func);
        }
        if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyEsc)
        {
            handleEscKey();
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->ClearAll();
            }
            mtcKeyEvent->setKeyType(KC_Func);
            ::showWarning(tr(""), true, FormWarning::WARNING);
        }
        beginDetectCard();

        if(mtcKeyEvent->isFuncKey())
        {
            if(mtcKeyEvent->func() == KeyCash ||
                    mtcKeyEvent->func() == KeyStoreCard)
            {
                mtcKeyEvent->setKeyType(KC_Func);
            }
        }
    }
    else
    {
        if(mtcKeyEvent->isFuncKey() && (mtcKeyEvent->func() == KeyCash || mtcKeyEvent->func() == KeyStoreCard) )
        {
            m_cardDetecter.Close();
        }
    }
}

void FormEntryVehInfo::hideEvent(QHideEvent *event)
{
    disconnect(getDeviceFactory()->getCardReader(DEV_CardReader1)->getDetectThread(),
               SIGNAL(hasOneCard(quint32,int)), this, SLOT(detectCard(quint32,int)));
    getDeviceFactory()->getCardReader(DEV_CardReader1)->StopCardDetection();
    m_cardDetecter.Close();
    ::showWarning(tr(""), true, FormWarning::WARNING);

}

void FormEntryVehInfo::showEvent(QShowEvent *)
{
    if(!m_bResultMode)
    {
        connect(getDeviceFactory()->getCardReader(DEV_CardReader1)->getDetectThread(),
                SIGNAL(hasOneCard(quint32,int)), this, SLOT(detectCard(quint32,int)));
        beginDetectCard();
    }
}

void FormEntryVehInfo::handleVehTypeKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    mtcKeyEvent->setKeyType(KC_VehClass);
    //如果输入货车车型，则必须要有计重信息
    if(IsTruck(mtcKeyEvent->vehType()))
    {
        //qDebug() << "veh count" << getWtDataMgr()->GetVehicleCount();
        //qDebug() << "use weight" << getLaneInfo()->getUseWeight();
        if(getWtDataMgr()->GetVehicleCount() <1 && getLaneInfo()->getUseWeight() && !getLaneInfo()->isEntryLane())
        {
            ::showWarning(tr("无计重信息，不能输入货车"), true, FormWarning::WARNING);
            return;
        }
    }

    ui->labEnVehType->setText(mtcKeyEvent->keyName());
    m_nVehType = mtcKeyEvent->vehType();
    if(ui->labEnVehClass->text().length() == 0)
    {
        ui->labEnVehClass->setText("普通车");
    }
    if(ui->labEnVehPlate->text().isEmpty())
    {
        ui->labEnVehPlate->setText(tr("无车牌"));
    }
    //保存车型信息
    m_bFirstInput = false;
    getLaneInfo()->setVehClass(mtcKeyEvent->vehType());
    if(getLaneInfo()->getVehType() == VC_None)
    {
        getLaneInfo()->setVehType(VT_Normal);
    }
    if(getLaneInfo()->getVehClass() == VC_None)
    {
        m_bFirstInput = true;
        getLaneCtrl()->getTollItem()->m_VehInfo.PVehClass = (CVehClass)getLaneInfo()->getVehClass();
        if(getLaneInfo()->getVehType() == VT_None)
        {
            getLaneCtrl()->getTollItem()->m_VehInfo.PVehType = (CVehType)getLaneInfo()->getVehType();
        }
        //保存首次输入车型的时间
        getLaneCtrl()->getTollItem()->setVTInputTime(QDateTime::currentDateTime());
    }
    getLaneCtrl()->getTollItem()->m_VehInfo.VehClass = (CVehClass)getLaneInfo()->getVehClass();
    //qDebug()<<"getLaneCtrl()->getTollItem()->m_VehInfo.VehClass"<<getLaneCtrl()->getTollItem()->m_VehInfo.VehClass;
    getLaneCtrl()->getTollItem()->m_VehInfo.VehType = (CVehType)getLaneInfo()->getVehType();

    getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VC);
    //如果为第一次输入车型数据，则跳转显示车牌修改页面确认车牌
    if(m_bFirstInput)
    {

    }
    ::showWarning(tr("请刷通行卡"), false, FormWarning::WARNING);
    if(getDeviceFactory()->getFD() != NULL)
    {
        getDeviceFactory()->getFD()->ClearAll();
    }
    if(getDeviceFactory()->getFD() != NULL)
    {
        //显示费显信息
        getDeviceFactory()->getFD()->ShowVehClass(
                    VehClass2Byte(getLaneCtrl()->getTollItem()->m_VehInfo.VehClass),
                    m_bFirstInput,
                    true);
    }
    //TODO:费显显示计重信息这里原来写的是ecount > 1
    if(getWtDataMgr()->GetVehicleCount() > 0 &&
            IsTruck(VehClass2Byte(getLaneCtrl()->getTollItem()->m_VehInfo.VehClass)))
    {
        TWtVehicle *vehInfo = NULL;
        bool ret = getWtDataMgr()->GetFirstVehAxisInfo(&vehInfo);
        if(ret)
        {
            if(getDeviceFactory()->getFD() != NULL)
            {
                getDeviceFactory()->getFD()->ShowWeightInfo(vehInfo->GetWeight(),
                                                            (((qint64)vehInfo->GetWeight() - (qint64)vehInfo->GetLimitWeight()) >= 0 ?
                                                                 ((qint64)vehInfo->GetWeight() - (qint64)vehInfo->GetLimitWeight()) : 0),
                                                            vehInfo->GetOverloadRate(), true);
            }
        }
    }
}

void FormEntryVehInfo::handleVehClassKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    ui->labVehClass->setText(mtcKeyEvent->getLogicKeyName());
}

void FormEntryVehInfo::handleEscKey()
{
    ui->label_2->clear();
    ui->label_3->clear();
    if(hasFinishVehInfo())
    {
        if(getLaneInfo()->getVehType() != VT_Normal)
        {
            getLaneInfo()->setVehType(VT_Normal);
            ui->labEnVehClass->setText(tr("普通车"));
        }
        else
        {
            getLaneInfo()->setVehType(VT_None);
            getLaneInfo()->setVehPlate("");
            getLaneInfo()->setVehClass(0);
            getLaneInfo()->clearCurVehInfo();
            //TODO_liujian
            getLaneCtrl()->getTollItem()->ClearSavedInfo();
            ui->labEnVehPlate->setStyleSheet("");
            ui->labEnVehClass->clear();
            ui->labEnVehPlate->clear();
            ui->labEnVehType->clear();
        }
    }
    else
    {
        //如果用户先选择特殊车辆，则此处要清空特殊车辆
        if(!ui->labEnVehClass->text().isEmpty())
        {
            ui->labEnVehClass->setText("");
            getLaneInfo()->setVehType(VT_Normal);
        }
    }
}

void FormEntryVehInfo::beginDetectCard()
{
    if(hasFinishVehInfo())
    {
        ::showWarning(tr("请刷通行卡"), false, FormWarning::WARNING);
        getDeviceFactory()->getCardReader(DEV_CardReader1)->StartCardDetection();
    }
}
QString getQssByPlateColor(int nColor)
{
    switch(nColor)
    {
    //黑字 白字
    case VP_COLOR_BLUE:
        return "background-color: rgb(0, 0, 255);color: rgb(255, 255, 255);";
        //黄色时对应黑字
    case VP_COLOR_YELLOW:
        return "background-color: rgb(255, 255, 0);color: rgb(0, 0, 0);";
        //白色时对应黑字
    case VP_COLOR_WHITE:
        return "background-color: rgb(255, 255, 255);color: rgb(0, 0, 0);";
        //黑色时对应白字
    case VP_COLOR_BLACK:
        return "background-color: rgb(0, 0, 0);color: rgb(255, 255, 255);";
    default:
        return "";
        break;
    }
}

void FormEntryVehInfo::getVprResult(int nIndex)
{
    VPRArm* pVpr = getDeviceFactory()->getVpr();
    const CVPRResult* pResult = pVpr->GetCurrentResult();
    ////qDebug() << tr("显示车牌") << pResult->sVehPlate << pResult->nColor;
    //如果此时没有车牌，或者显示无车牌
    if(ui->labEnVehPlate->text().isEmpty() || ui->labEnVehPlate->text().compare(tr("无车牌")) == 0)
    {
        //显示自动识别车牌
        ui->labEnVehPlate->setText( GetVehColorQString(pResult->nColor) + pResult->sVehPlate);
        ui->labEnVehPlate->setStyleSheet(getQssByPlateColor(pResult->nColor));
        m_nPlateColor = pResult->nColor;
        getLaneInfo()->setVehPlateColor(m_nPlateColor);
        getLaneInfo()->setVehPlate(pResult->sVehPlate);
        getLaneCtrl()->getTollItem()->m_VehInfo.nVehPlateColor = getLaneInfo()->getVehPlateColor();
    }
}

void FormEntryVehInfo::detectCard(quint32 nReadId, int nCardType)
{
    if(!getEntryCardProcessor()->Prompt())
    {
        return;
    }
    if(!this->hasFinishVehInfo())
    {
        return;
    }
    //停止检测卡片
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StopCardDetection();
    //保存卡类型
    //    getCardProcessor()->SetPhyCardType(nCardType);
    if(getLaneInfo()->isEntryLane())
    {
        getEntryCardProcessor()->SetPhyCardType(nCardType);
    }
    else
    {
        getCardProcessor()->SetPhyCardType(nCardType);
    }
    bool bRlt = false;
    ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();

    if(nCardType == TYPE_PRO)
    {
        //        pExit->setExceptCard(false);
        //        bRlt = getCardProcessor()->ProcessOpenCardEvent_ProCard(DEV_CardReader1);
        //入口处理
        if(getLaneInfo()->isEntryLane())
        {
            bRlt = getEntryCardProcessor()->Processopencardevent_Procard(DEV_CardReader1);
        }
        else
        {
            ExitTollItem* pExit = (ExitTollItem*)getLaneCtrl()->getTollItem();
            pExit->setExceptCard(false);
            bRlt = getCardProcessor()->ProcessOpenCardEvent_ProCard(DEV_CardReader1);
        }
    }
    else if(TYPE_M1_S50_BLOCK == nCardType || TYPE_M1_S50_MAD == nCardType ||
            TYPE_M1_S70_BLOCK == nCardType || TYPE_M1_S70_MAD == nCardType)
    {
        //判断没有上卡盒
        if(!getDataFileMgr()->IsLoaded())
        {
            ::showWarning(tr("未上卡盒，不允许刷通行卡"));
            pReader->StartCardDetection();
            return;
        }

        //        bRlt = getCardProcessor()->ProcessOpenCardEvent_TollCard(DEV_CardReader1);
        if(getLaneInfo()->isEntryLane())
        {
            bRlt = getEntryCardProcessor()->ProcessOpenCardEvent_TollCard(DEV_CardReader1);
        }
        else
        {
            bRlt = getCardProcessor()->ProcessOpenCardEvent_TollCard(DEV_CardReader1);
        }
    }
    if(!bRlt)
    {
        //继续检测卡片
        pReader->StartCardDetection();
    }
    else
    {
        emit detectCardSuccess();
    }
    this->setFocus();

}

