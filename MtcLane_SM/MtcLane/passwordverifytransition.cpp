#include "passwordverifytransition.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qoperparamfile.h"
#include "CommonModule/laneutils_new.h"
#include "MtcKey/MtcLaneState.h"
#include "formpasswordverify.h"
#include "formmainwindow.h"
#include "formwarning.h"
#include "laneinfo.h"
#include "ParamModule/qparammgr.h"
#include "ParamModule/qshiftparamfile.h"
#include "formtablewidget.h"
#include "formmtctitle.h"
PasswordVerifyTransition::PasswordVerifyTransition(QObject *sender, const char *signal):
    MtcKeySignalTransition(sender, signal)
{

}

PasswordVerifyTransition::PasswordVerifyTransition(QState *state):
    MtcKeySignalTransition(state)
{

}

void PasswordVerifyTransition::onMtcKeyTransition(const MtcKeyPropertity &keyPropertity, int keyType)
{
    //清空警告
    ::showWarning("");
    //清除用户名和密码
    FormPasswordVerify* pPassword = (FormPasswordVerify*)((MtcLaneState*)sourceState())->GetWindowPtr();
    pPassword->clearOperInfo();
    showShiftInfo();
}

void PasswordVerifyTransition::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    //如果用户按下确认键，则开始确认
    if(keyPropertity.isFuncKey() && keyPropertity.func == KeyConfirm)
    {
        FormPasswordVerify* pPassword = (FormPasswordVerify*)((MtcLaneState*)sourceState())->GetWindowPtr();
        QString sPassword = pPassword->getPassword();
        QString sOper = pPassword->getOper();
        QString sError;
        SOperInfoValue operValue;
        bool ok = verifyPassword(sOper, sPassword, sError, operValue);
        if(ok)
        {
            //将数据保存到Laneinfo
            getLaneInfo()->setOper(operValue);
            *keyType = KC_Func;
        }
        else
        {
            ::showWarning(sError);
            return;
        }
    }
}

bool PasswordVerifyTransition::verifyPassword(const QString &sOperName, const QString &sPassword, QString& sError, SOperInfoValue& operValue)
{
    if(sOperName.isEmpty() || sOperName.length() != 8)
    {
        sError = tr("工号长度错误，请输入8位工号");
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
        if(sMd5.compare(operInfo.szPassWord, Qt::CaseInsensitive) == 0)
        {
            operValue = operInfo;
            return true;
        }
    }
    sError = tr("工号或密码输入错误");
    return false;
}

void PasswordVerifyTransition::showShiftInfo()
{
    //获取可用工班信息
    QShiftParamFile* pFile = (QShiftParamFile*)getParamMgr()->GetParamFile(cfShift);
    QList< SShiftValue > shiftArray;
    pFile->getShiftInfoByTime(QDateTime::currentDateTime(),shiftArray);

    //设置工班显示信息
    FormTableWidget* pTable = (FormTableWidget*)(((MtcLaneState*)targetState())->GetWindowPtr());
    pTable->setCol(1);
    QList<SShiftValue>::const_iterator it = shiftArray.begin();
    QString sShift;
    for(; it != shiftArray.end(); it++)
    {
        sShift = getShiftString(it->sShiftName, it->iWorkDate);
        pTable->insertItem(sShift, QVariant(it->wShift));

    }
}

QString PasswordVerifyTransition::getShiftString(const QString& sShiftName, const QDate &workDate)
{
    return tr("%1 %2").arg(workDate.toString("yyyy-MM-dd")).arg(sShiftName);
}
