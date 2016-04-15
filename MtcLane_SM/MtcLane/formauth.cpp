#include "formauth.h"
#include "ui_formauth.h"
#include "lanectrl.h"
#include "formmainwindow.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qoperparamfile.h"
#include "MtcKey/MtcKeyDef.h"
#include "formmessage.h"
#include "laneinfo.h"
FormAuth::FormAuth(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormAuth)
{
    ui->setupUi(this);
    m_nAuthTimerId = -1;
    m_nCountTimerId = -1;
    m_nCount = 20;
    m_monType = mqUnKnow;
    m_nAuthType = 0;
    m_nAuthName = "";
    connect(getMon(), SIGNAL(OnHKResponse(THKResultInfo)),
            this, SLOT(AuthResponse(THKResultInfo)));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    filterChildrenKeyEvent();

}

FormAuth::~FormAuth()
{
    delete ui;
}


void FormAuth::setMonType(TMonReqID monType)
{
    m_monType = monType;
}

void FormAuth::setAuthType(int nType)
{
    m_nAuthType = nType;
}

void FormAuth::timerEvent(QTimerEvent *event)
{
    if(m_nAuthTimerId == event->timerId())
    {
        //转到授权界面
        getMon()->CancelHK();
        ui->stackedWidget->setCurrentIndex(1);
        setFocus();
    }
    if(m_nCountTimerId == event->timerId())
    {
        //倒计时时间-1
        m_nCount--;
        ui->label_3->setText(QString::number(m_nCount));
    }
}

void FormAuth::showEvent(QShowEvent *event)
{
    if(m_nAuthType == 0)
    {
        ui->stackedWidget->setCurrentIndex(0);
        setFocus();
        clearResult();
        //启动监控20秒定时器
        m_nAuthTimerId = startTimer(20000);
        m_nCountTimerId = startTimer(1000);

    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
        setFocus();
        clearResult();

    }
    //请求监控
    getLaneCtrl()->getTollItem()->SendRequestMonInfo(m_monType);
    ::showWarning(tr(""), true, FormWarning::WARNING);

}

void FormAuth::clearResult()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    m_nCount = 20;
    ui->label_3->setText(QString::number(m_nCount));
}

void FormAuth::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        handleFuncKey(mtcKeyEvent);
    }
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Func);
        handleNumberKey(mtcKeyEvent);
    }
}

void FormAuth::handleFuncKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    mtcKeyEvent->setKeyType(KC_Func);
    if(mtcKeyEvent->func() == KeyEsc)
    {
        //getMon()->CancelHK();
        setResult(0);
    }
    if(ui->stackedWidget->currentIndex() == 1)
    {
        static int times = 0;
        if(mtcKeyEvent->func() == KeyConfirm)
        {
            //liujian
            if(ui->lineEdit->hasFocus())
            {
                ui->lineEdit_2->setFocus();
                return;
            }
            //验证密码是否有权限
            if(VerifyAuth(ui->lineEdit->text(), ui->lineEdit_2->text()))
            {
                m_nResult = 1;
                AuthClose();
                //liujian
                times = 0;
            }
            else
            {
                times++;
            }
            if(times == 3)
            {
                m_nResult = 0;
                times = 0;
                AuthClose();
            }
        }
        if(mtcKeyEvent->func() == KeyDel)
        {
            if(ui->lineEdit->hasFocus())
            {
                ui->lineEdit->setText(ui->lineEdit->text().left(ui->lineEdit->text().length() - 1));
            }
            else
            {
                if(ui->lineEdit_2->text().isEmpty())
                {
                    ui->lineEdit->setFocus();
                    ui->lineEdit->setText(ui->lineEdit->text().left(ui->lineEdit->text().length() - 1));
                }
                else
                {
                    ui->lineEdit_2->setText(ui->lineEdit_2->text().left(ui->lineEdit_2->text().length() - 1));
                }
            }
        }
    }
}

void FormAuth::handleNumberKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        if(!ui->lineEdit_2->hasFocus())
            ui->lineEdit->setFocus();
        if(ui->lineEdit->text().length() < 9 && ui->lineEdit->hasFocus())
        {
            //liujian
            //ui->lineEdit->setFocus();
            ui->lineEdit->setText(ui->lineEdit->text().append(mtcKeyEvent->getLogicKeyName()));
        }
        else
        {
            ui->lineEdit_2->setFocus();
            ui->lineEdit_2->setText(ui->lineEdit_2->text().append(mtcKeyEvent->getLogicKeyName()));
        }
        if(ui->lineEdit->text().length() == 9)
        {
            ui->lineEdit_2->setFocus();
        }
    }
}

bool FormAuth::VerifyAuth(const QString &sOper, const QString &sPwd)
{
    QString sError;
    bool typeok = VerifyOperType(sOper, sError);
    if(typeok)
    {
        bool ok = VerifyPwd(sOper, sPwd, sError);
        if(ok)
        {
            //liujian
            TAuthorizeInfo AutoInfo;
            AutoInfo.clear();
            AutoInfo.AuthType = 1;
            memcpy(AutoInfo.AuthID,sOper.toLocal8Bit().data(),sizeof(AutoInfo.AuthID));
            memcpy(AutoInfo.AuthName,m_nAuthName.toLocal8Bit().data(),sizeof(AutoInfo.AuthName));
            AutoInfo.AuthTime = QDateTime::currentDateTime();
            getLaneCtrl()->getTollItem()->SetAuthInfo(AutoInfo);
            return true;
        }
    }
    ::showWarning(sError, true, FormWarning::WARNING);
    return false;
}

bool FormAuth::VerifyOperType(const QString &sOper, QString &sError)
{
    QOperParamFile* pFile = (QOperParamFile*)getParamMgr()->GetParamFile(cfOper);
    SOperInfoValue operInfo;
    bool ok = pFile->getOperInfoByOperNo(sOper.toUInt(),operInfo);
    if(ok)
    {
        if(operInfo.szFunRole[1] == '1')
        {
            m_nAuthName = operInfo.sOperName;
            return true;
        }
        else
        {
            sError = tr("无班长权限,请刷班长卡");
            return false;
        }
    }else
    {
        sError = tr("工号输入错误");
        return false;
    }
}

bool FormAuth::VerifyPwd(const QString &sOperName, const QString &sPassword, QString& sError)
{
    if(sOperName.isEmpty())
    {
        sError = tr("请输入工号");
        return false;
    }
    if(sPassword.isEmpty())
    {
        sError = tr("请输入密码");
        return false;
    }
    QOperParamFile* pFile = (QOperParamFile*)getParamMgr()->GetParamFile(cfOper);
    SOperInfoValue operInfo;
    bool ok = pFile->getOperInfoByOperNo(sOperName.toUInt(),operInfo);
    if(ok)
    {
        QString sMd5 = GetMD5ForBuffer(sPassword.toLocal8Bit().data(), sPassword.length());
        if(sMd5.compare(operInfo.szPassWord,Qt::CaseInsensitive) == 0)
        {
            return true;
        }
        else
        {
            sError = tr("密码输入错误");
            return false;
        }
    }
    sError = tr("工号输入错误");
    return false;
}

void FormAuth::AuthClose()
{
    this->hide();
    m_pEventLoop->quit();
    m_pEventLoop = 0;
}

void FormAuth::AuthResponse(THKResultInfo info)
{
    //QDebug() << "verify form auth";
    if(m_nAuthTimerId != -1)
    {
        killTimer(m_nAuthTimerId);
        m_nAuthTimerId  = -1;
    }
    switch(info.Response)
    {
    case hrAccept:
        m_nResult = 1;
        AuthClose();
        break;
    case hrRefuse:
        m_nResult = 0;
        AuthClose();
        break;
    case hrTimeout:
    case hrFail:
        ui->stackedWidget->setCurrentIndex(1);
        break;
    default:
        break;
    }
    getMon()->CancelHK();
}

void FormAuth::handleOperCard(quint32 nReadId, int nCardType)
{
    //停止卡检测线程
    CardDetectThread* thread = (CardDetectThread*)sender();
    QCardReader* pReader = thread->getCardReader();
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
FormAuth* getAuth(QWidget *widget, QLayout *layout)
{
    static FormAuth* auth = NULL;
    if(auth == NULL)
    {
        auth = new FormAuth(widget, layout);
    }
    return auth;
}

void FormAuth::on_stackedWidget_currentChanged(int arg1)
{
    if(!isHidden() && arg1 == 1)
    {
        //班长授权，打开卡读写线程
        //启动卡检测线程
        if(getLaneInfo()->isMtcLane())
        {
            QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
            pReader->StartCardDetection();
            connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
                    this, SLOT(handleOperCard(quint32,int)));
            ::showWarning(tr(""), true, FormWarning::WARNING);
        }
        else if(getLaneInfo()->isAutoLane())
        {
            for(int i = DEV_CardReader3; i <= DEV_CardReader4; i++)
            {
                QCardReader* pReader = getDeviceFactory()->getCardReader((DEVNO)i);
                pReader->StartCardDetection();
                connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
                        this, SLOT(handleOperCard(quint32,int)));
            }
        }
    }
}


bool FormAuth::verifyOperCard(quint32 dwOperId, quint32 dwOperCardId)
{
    QOperParamFile* pFile = (QOperParamFile*)getParamMgr()->GetParamFile(cfOper);
    QString sError;
    SOperInfoValue tmpValue;
    if(!pFile->verifyOperCard(dwOperId, dwOperCardId, tmpValue, sError))
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
    if(tmpValue.szFunRole[1] == '1')
    {
        //liujian
        TAuthorizeInfo AutoInfo;
        AutoInfo.clear();
        AutoInfo.AuthType = 1;
        memcpy(AutoInfo.AuthID,QString::number(tmpValue.dwOper).toLocal8Bit().data(),sizeof(AutoInfo.AuthID));
        QString tmpstr = tmpValue.sOperName;
//        Utf8toGB2312(tmpstr.toLocal8Bit().data(),tmpstr.length());
//        memcpy(AutoInfo.AuthName,tmpValue.sOperName.toLocal8Bit().data(),sizeof(AutoInfo.AuthName));
        memcpy(AutoInfo.AuthName,Utf8toGB2312(tmpstr.toLocal8Bit().data(),tmpstr.length()).data(),sizeof(AutoInfo.AuthName));
        AutoInfo.AuthTime = QDateTime::currentDateTime();
        getLaneCtrl()->getTollItem()->SetAuthInfo(AutoInfo);
        return true;
    }
    else
    {

        ::showWarning(tr("无班长权限，请重新输入班长工号或重新刷班长卡"), true, FormWarning::WARNING);
        return false;
    }
}

void FormAuth::hideEvent(QHideEvent *)
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
        pReader->StopCardDetection();
        disconnect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
                   this, SLOT(handleOperCard(quint32,int)));
    }
}

bool FormAuth::verifyBasicInfo(const TOpCardBasicInfo& opCardBasicInfo)
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
    operCardId = opCardBasicInfo.M1BasicInfo.dwCardNum;
    if(!verifyOperCard(operId, operCardId))
    {
        return false;
    }
    return true;
}
void FormAuth::focusInEvent(QFocusEvent *ev)
{
    ui->lineEdit->setFocus();
}
