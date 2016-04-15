#include "formpasswordverify.h"
#include "ui_formpasswordverify.h"
#include "formmainwindow.h"
#include "ParamModule/qoperparamfile.h"
#include "ParamModule/qparammgr.h"
#include "CommonModule/laneutils_new.h"
#include "laneinfo.h"
FormPasswordVerify::FormPasswordVerify(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormPasswordVerify), m_bCount(0)
{
    ui->setupUi(this);
    this->filterChildrenKeyEvent();
    m_shiftType = LOGIN;
    operTypeFalse = false;
}

FormPasswordVerify::~FormPasswordVerify()
{
    delete ui;
}

void FormPasswordVerify::clearOperInfo()
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
}


void FormPasswordVerify::focusInEvent(QFocusEvent *ev)
{
    ui->lineEdit->setFocus();
}

void FormPasswordVerify::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        handleNumberKey(mtcKeyEvent);
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyDel)
    {
        handleEraserKey(mtcKeyEvent);
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyEsc)
    {
        if(m_pEventLoop != 0)
        {
            setResult(0);
        }
    }
    if(mtcKeyEvent->isFuncKey() && mtcKeyEvent->func() == KeyConfirm)
    {
        if(ui->lineEdit->hasFocus())
        {
            ui->lineEdit_2->setFocus();
        }
        else
        {
            bool rlt = verifyPassword();
            //如果验证3次不正确，则返回初始状态
            if(!rlt)
            {
                if(operTypeFalse)
                {
                    setResult(0);
                    getMsgWidget()->setMessage(tr("请放置身份卡\n按『上/下班』键登录"));

                }
                else
                {
                    m_bCount++;
                    if(m_bCount == 3 /*|| operTypeFalse*/)
                    {
                        setResult(0);
                        m_bCount = 0;
                    }
                }
            }
            else
            {
                m_bCount = 0;
                setResult(1);
            }
        }

    }
    mtcKeyEvent->setKeyType(KC_Func);
}

void FormPasswordVerify::handleNumberKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    mtcKeyEvent->setKeyType(KC_Number);
    if(m_shiftType == LOGIN)
    {
        if(ui->lineEdit->hasFocus())
        {
            ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
            if(ui->lineEdit->text().length() == 9)
            {
                ui->lineEdit_2->setFocus();
                ui->containerTitle->setText(tr("输入密码后按【确认】键上班"));
            }
        }
        else
        {
            ui->lineEdit_2->setText(ui->lineEdit_2->text().append(mtcKeyEvent->getLogicKeyName()));
        }
    }
    else
    {
        ui->lineEdit_2->setText(ui->lineEdit_2->text().append(mtcKeyEvent->getLogicKeyName()));
    }
}

void FormPasswordVerify::handleEraserKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    mtcKeyEvent->setKeyType(KC_Func);
    if(m_shiftType == LOGIN)
    {
        //如果密码有焦点
        if(ui->lineEdit_2->hasFocus())
        {
            if(ui->lineEdit_2->text().isEmpty())
            {
                ui->lineEdit->setFocus();
                ui->containerTitle->setText(tr("请输入工号\n按【确认】键切换密码输入"));
            }
            else
            {
                ui->lineEdit_2->setText(ui->lineEdit_2->text().left(ui->lineEdit_2->text().length() - 1));
            }
        }
        if(ui->lineEdit->hasFocus())
        {
            if(!ui->lineEdit->text().isEmpty())
            {
                ui->lineEdit->setText(ui->lineEdit->text().left(ui->lineEdit->text().length() - 1));
            }
        }
    }
    else
    {
        if(!ui->lineEdit_2->text().isEmpty())
        {
            ui->lineEdit_2->setText(ui->lineEdit_2->text().left(ui->lineEdit_2->text().length() - 1));
        }
    }
}

void FormPasswordVerify::showEvent(QShowEvent *event)
{
    m_nResult = 0;
    //启动卡检测线程
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StartCardDetection();
    connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
            this, SLOT(handleOperCard(quint32,int)));
    ::showWarning(tr(""), true, FormWarning::WARNING);
    m_bCount = 0;

}

void FormPasswordVerify::hideEvent(QHideEvent *event)
{
    clearOperInfo();
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StopCardDetection();
    disconnect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
               this, SLOT(handleOperCard(quint32,int)));
    ::showWarning(tr(""), true, FormWarning::WARNING);
}

bool FormPasswordVerify::verifyPassword()
{
    QString sOper = ui->lineEdit->text();
    QString sPassword = ui->lineEdit_2->text();
    if(sOper.isEmpty())
    {
        ::showWarning(tr("工号长度错误"), true, FormWarning::WARNING);
        return false;
    }
    if(sPassword.isEmpty())
    {
        ::showWarning(tr("请输入密码"), true, FormWarning::WARNING);
        return false;
    }
    QOperParamFile* pFile = (QOperParamFile*)getParamMgr()->GetParamFile(cfOper);
    SOperInfoValue operInfo;
    bool ok = pFile->getOperInfoByOperNo(sOper.toUInt(),operInfo);
    if(ok)
    {
        QString sMd5 = GetMD5ForBuffer(sPassword.toLocal8Bit().data(), sPassword.length());
        if(sMd5.compare(operInfo.szPassWord, Qt::CaseInsensitive) == 0)
        {
            if(operInfo.szFunRole[0] == '1')
            {
                getLaneInfo()->setOper(operInfo);
                return true;
            }
            else
            {
                this->hide();
                operTypeFalse = true;
                getMsgWidget()->setCloseTimer(3000);
                getMsgWidget()->setMessage(tr("无发卡权限，无法上班"));
                ::showWarning(tr("按【确认】或【取消】键关闭"), true, FormWarning::WARNING);
                getMsgWidget()->doModalShow();
                this->show();
                this->setFocus();
                return false;
            }
        }
        else
        {
            ::showWarning(tr("密码不正确"), true, FormWarning::WARNING);
            return false;
        }
    }
    ::showWarning(tr("无此操作员"), true, FormWarning::WARNING);
    return false;
}

bool FormPasswordVerify::verifyBasicInfo(const TOpCardBasicInfo& opCardBasicInfo)
{
    quint32 operId = 0;
    quint32 operCardId = 0;
    if(opCardBasicInfo.M1BasicInfo.bType != CARD_TYPE_OP_CARD)
    {
        ::showWarning(tr("非身份卡，请刷身份卡！"),true, FormWarning::WARNING);
        return false;
    }
    if(opCardBasicInfo.M1BasicInfo.wNetworkId != ORG_NETWORKID)
    {
        ::showWarning(tr("非本网络卡！"),true, FormWarning::WARNING);
        return false;
    }
    else if(opCardBasicInfo.dwOpId == 0 || opCardBasicInfo.M1BasicInfo.dwCardNum == 0)
    {
        ::showWarning(tr("无效身份卡！"),true, FormWarning::WARNING);
        return false;
    }
    operId = opCardBasicInfo.dwOpId;
    //qDebug() << "opCardBasicInfo.dwOpId" <<opCardBasicInfo.dwOpId;
    //qDebug() << "getLaneInfo()->getOper().dwOper" <<getLaneInfo()->getOper().dwOper;
    operCardId = opCardBasicInfo.M1BasicInfo.dwCardNum;
    if(m_shiftType == LOGOUT)
    {
        if(operId != getLaneInfo()->getOper().dwOper)
        {
            ::showWarning(tr("非上班员工身份卡！"));
            return false;
        }
    }
    if(!verifyOperCard(operId, operCardId))
    {
        return false;
    }
    return true;
}

bool FormPasswordVerify::verifyOperCard(quint32 dwOperId, quint32 dwOperCardId)
{
    QOperParamFile* pFile = (QOperParamFile*)getParamMgr()->GetParamFile(cfOper);
    QString sError;
    if(!pFile->verifyOperCard(dwOperId, dwOperCardId, m_operValue, sError))
    {
        //如果是卡号不符，则将用户号显示出来，如果是无此操作员，都开始提示
        if(sError.compare(tr("卡号不符")) == 0)
        {
            ui->lineEdit->setText(QString::number(dwOperId));
        }
        else
        {
            ::showWarning(sError);
        }
        return false;
    }
    //判断权限
    if(m_operValue.szFunRole[0] == '1')
    {
        return true;
    }
    else
    {
        this->hide();
        operTypeFalse = true;
        getMsgWidget()->setCloseTimer(3000);
        getMsgWidget()->setMessage(tr("无发卡权限，无法上班"));
        ::showWarning(tr("按【确认】或【取消】键关闭"), true, FormWarning::WARNING);
        getMsgWidget()->doModalShow();
        this->show();
        this->setFocus();
        return false;
    }
}

void FormPasswordVerify::handleOperCard(quint32 /*nCardId*/, int nCardType)
{
    //停止卡检测线程
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StopCardDetection();
    //比对卡类型信息
    if(nCardType == TYPE_M1_S50_BLOCK ||
            nCardType == TYPE_M1_S50_MAD ||
            nCardType == TYPE_M1_S70_BLOCK||
            nCardType == TYPE_M1_S70_MAD
            )
    {
        TOpCardBasicInfo opCardBasicInfo;
        TOpCardAppInfo opCardAppInfo;

        if(!pReader->ReadOpCard(opCardBasicInfo, opCardAppInfo))
        {
            ::showWarning(tr("读身份卡失败！"), true, FormWarning::WARNING);
            pReader->StartCardDetection();
        }
        if(verifyBasicInfo(opCardBasicInfo))
        {
            getLaneInfo()->setOper(m_operValue);
            //m_nResult = 1;
            setResult(1);

        }
        else
        {
            pReader->StartCardDetection();
        }
    }
    else
    {
        ::showWarning(tr("非身份卡"),true, FormWarning::WARNING);
        pReader->StartCardDetection();
    }
}

void FormPasswordVerify::showType(FormPasswordVerify::emShiftType type)
{
    if(type == LOGOUT)
    {
        //自动设置员工号并隐藏
        ui->lineEdit_2->setFocus();
        ui->lineEdit->setText(tr("%1").arg(getLaneInfo()->getOper().dwOper, 9,10, QLatin1Char('0')));
        ui->lineEdit->hide();
        ui->label_4->hide();
        ui->containerTitle->setText(tr("下班确认"));
    }
    else
    {
        ui->lineEdit->clear();
        ui->label_4->show();
        ui->lineEdit->show();
        ui->containerTitle->setText(tr("请输入工号密码登录"));
    }
    m_shiftType = type;
}


FormPasswordVerify* getPasswordVerify(QWidget *widget, QLayout *layout)
{
    static FormPasswordVerify* p = NULL;
    if(p == NULL)
    {
        p = new FormPasswordVerify(widget, layout);
    }
    return p;
}
