#include "loadparamtransition.h"
#include "ParamModule/qparamfilemgr.h"
#include "ParamModule/qsysparamfile.h"
#include "MtcKey/MtcLaneState.h"
#include "globalinfo.h"
#include <QTextCodec>
void LoadParamTransition::onTransition(QEvent *event)
{
    //保存站信息
    QSysParamFile * sysFile = (QSysParamFile*)QParamFileMgr::getParamFileMgr()->GetFileByType(cfSysParaDic);
    QString station = sysFile->GetSysParamInfo().m_sStationName;
    GlobalInfo::getGlobalInfo()->setProperty("station", station);
}

bool LoadParamTransition::eventTest(QEvent *event)
{
    if(!QSignalTransition::eventTest(event))
    {
//        //判断原状态是否为初始状态
//        if(sourceState()->property("status").toString().compare("initState") == 0)
//        {
            return false;
        //}
    }
    qDebug() << "loading param file";
    QParamFileMgr::getParamFileMgr()->init(2);
    QParamFileMgr::getParamFileMgr()->SetWindowPtr(
                ((MtcLaneState *)this->sourceState())->GetWindowPtr());
    return QParamFileMgr::getParamFileMgr()->CheckAndLoadParamFile();
    //QStateMachine::SignalEvent *se = static_cast<QStateMachine::SignalEvent*>(event);
}

