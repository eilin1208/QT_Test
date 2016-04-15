#include "qcardreader.h"
#include <QLibrary>
#include "CommonModule/laneutils_new.h"
#include <QDateTime>

const int MAX_LEN = 128;
QCardReader::QCardReader(const QString& sName,QObject *parent) :
    DevBaseInterface(sName,parent)
{
    m_detectThread.SetCardReader(this);
    JT_OpenReader=NULL;
    JT_CloseReader=NULL;
    JT_OpenCard=NULL;
    JT_CloseCard=NULL;
    JT_LEDDisplay=NULL;
    JT_AudioControl=NULL;
    JT_ReadBlock=NULL;
    JT_WriteBlock=NULL;
    JT_WriteKey=NULL;
    JT_CPUCCommand=NULL;
    JT_SamReset=NULL;
    JT_ChangeSamBaudrate=NULL;
    JT_SamCommand=NULL;
    JT_ReadFile=NULL;
    JT_WriteFile=NULL;
    JT_GetLastError=NULL;
    JT_ReaderVersion=NULL;
}

bool QCardReader::LoadFunction()
{
    if(m_bDriverLoaded)
    {
        // 多个类可能使用一套动态库，应避免重复加载
        return true;
    }

    JT_OpenReader=(Func_JT_OpenReader)m_qLibrary.resolve("JT_OpenReader");
    JT_CloseReader=(Func_JT_CloseReader)m_qLibrary.resolve("JT_CloseReader");
    JT_OpenCard=(Func_JT_OpenCard)m_qLibrary.resolve("JT_OpenCard");
    JT_CloseCard=(Func_JT_CloseCard)m_qLibrary.resolve("JT_CloseCard");
    JT_LEDDisplay=(Func_JT_LEDDisplay)m_qLibrary.resolve("JT_LEDDisplay");
    JT_AudioControl=(Func_JT_AudioControl)m_qLibrary.resolve("JT_AudioControl");
    JT_ReadBlock=(Func_JT_ReadBlock)m_qLibrary.resolve("JT_ReadBlock");
    JT_WriteBlock=(Func_JT_WriteBlock)m_qLibrary.resolve("JT_WriteBlock");
    JT_WriteKey=(Func_JT_WriteKey)m_qLibrary.resolve("JT_WriteKey");
    JT_CPUCCommand=(Func_JT_CPUCCommand)m_qLibrary.resolve("JT_CPUCCommand");
    JT_SamReset=(Func_JT_SamReset)m_qLibrary.resolve("JT_SamReset");
    JT_ChangeSamBaudrate=(Func_JT_ChangeSamBaudrate)m_qLibrary.resolve("JT_ChangeSamBaudrate");
    JT_SamCommand=(Func_JT_SamCommand)m_qLibrary.resolve("JT_SamCommand");
    JT_ReadFile=(Func_JT_ReadFile)m_qLibrary.resolve("JT_ReadFile");
    JT_WriteFile=(Func_JT_WriteFile)m_qLibrary.resolve("JT_WriteFile");
    JT_GetLastError=(Func_JT_GetLastError)m_qLibrary.resolve("JT_GetLastError");
    JT_ReaderVersion=(Func_JT_ReaderVersion)m_qLibrary.resolve("JT_ReaderVersion");
    if ((NULL==JT_OpenReader) ||
            (NULL==JT_CloseReader) ||
            (NULL==JT_OpenCard) ||
            (NULL==JT_CloseCard) ||
            (NULL==JT_LEDDisplay) ||
            (NULL==JT_AudioControl) ||
            (NULL==JT_ReadBlock) ||
            (NULL==JT_WriteBlock) ||
            (NULL==JT_WriteKey) ||
            (NULL==JT_CPUCCommand) ||
            (NULL==JT_SamReset) ||
            (NULL==JT_ChangeSamBaudrate) ||
            (NULL==JT_SamCommand) ||
            (NULL==JT_ReadFile) ||
            (NULL==JT_WriteFile) ||
            (NULL==JT_GetLastError) ||
            (NULL==JT_ReaderVersion))

    {
        LogMsg("lane",tr("获取卡读写器动态库[%1]中的一个或多个函数错误！").arg(m_sLibName));
        return false;
    }
    m_bDriverLoaded=true;
    return true;


}

bool QCardReader::ReleaseFunction()
{
    // 标记动态库未加载
    m_bDriverLoaded=false;
    // 函数指针置为NULL
    JT_OpenReader=NULL;
    JT_CloseReader=NULL;
    JT_OpenCard=NULL;
    JT_CloseCard=NULL;
    JT_LEDDisplay=NULL;
    JT_AudioControl=NULL;
    JT_ReadBlock=NULL;
    JT_WriteBlock=NULL;
    JT_WriteKey=NULL;
    JT_CPUCCommand=NULL;
    JT_SamReset=NULL;
    JT_ChangeSamBaudrate=NULL;
    JT_SamCommand=NULL;
    JT_ReadFile=NULL;
    JT_WriteFile=NULL;
    JT_GetLastError=NULL;
    JT_ReaderVersion=NULL;
    return true;
}

bool QCardReader::Init()
{
    int nRlt;
        char buf[MAX_LEN];
        char szReaderVersion[MAX_LEN];
        char szAPIVersion[MAX_LEN];
        if(!m_bDriverLoaded)
        {
            return false;
        }
#ifdef Q_OS_WIN
        m_nComID=ExtractComId(m_sConStr1.toLocal8Bit().data());
#else
        m_nComID=ExtractComId(m_sConStr2.toLocal8Bit().data());
#endif
        if(m_nComID<=0)
        {
            LogMsg("lane",tr("卡读写器[%1]串口号[%2]设置错误").arg(m_nReaderId).arg(m_sConStr1.toLocal8Bit().data()));
            return false;
        }
        //Close();
        // 打开卡读写器，建立串口连接
#ifdef Q_OS_WIN
        nRlt=JT_OpenReader(m_nComID,m_sConStr2.toLocal8Bit().data());
#else
        nRlt=JT_OpenReader(m_sConStr1.toLocal8Bit().data(),m_nComID,115200);
#endif

        if(0!=nRlt)
        {
            memset(buf,0, MAX_LEN);
            GetReaderLastError(buf, MAX_LEN-1);
            LogMsg("lane",tr("打开卡读写器[%1]失败:[%2],Reader Error Code=[%3]").arg(m_nReaderId).arg(buf).arg(nRlt));
            return false;
        }
        if(!InitPsam())
        {
            LogMsg("lane",tr("初始化卡读写器[%1]卡失败").arg(m_nReaderId));
            return false;
        }
        GetReaderVersion(szReaderVersion, MAX_LEN, szAPIVersion, MAX_LEN);
        LogMsg("lane",tr("打开卡读写器[%1]成功,读写器版本[%2],接口库版本[%3]").arg(m_nReaderId).arg(szReaderVersion).arg(szAPIVersion));
        m_bIsOpen = true;
        return true;
}

bool QCardReader::Close()
{
    //关闭卡检测线程
    if(m_detectThread.isRunning())
    {
        m_detectThread.Stop();
    }
    if(!m_bDriverLoaded)
    {
        return false;
    }
    JT_CloseReader(m_nComID);
    return true;
}

int QCardReader::GetReaderLastError(char *sLastError, int iMaxLength)
{
    if(( ! m_bDriverLoaded)||(JT_GetLastError == NULL))
    {
        if( sLastError && iMaxLength > 0)
        {
            //将字符串清空
            memset(sLastError,0,iMaxLength);
        }
        return -1;
    }

    return JT_GetLastError(m_nComID,sLastError,iMaxLength);
}

bool QCardReader::InitPsam()
{
    bool bRet1 = false,bRet2 = false;
    bRet1 = InitOldPsam(OLD_PSAM_SLOT);
    bRet2 = InitNewPsam(NEW_PSAM_SLOT);
    return (bRet1||bRet2);
}
void QCardReader::GetReaderVersion(char *sReaderVersion, int iRVerMaxLength, char *sAPIVersion, int iAPIVerMaxLength)
{
    if((!m_bDriverLoaded)||(JT_ReaderVersion == NULL))
    {
        if(sReaderVersion && iRVerMaxLength>0)
        {
            memset(sReaderVersion,0,iRVerMaxLength);
        }
        if(sAPIVersion && iAPIVerMaxLength > 0)
        {
            memset(sAPIVersion,0,iAPIVerMaxLength);
        }
        return ;
    }
    JT_ReaderVersion(m_nComID,sReaderVersion,iRVerMaxLength,sAPIVersion,iAPIVerMaxLength);
}

bool QCardReader::IsValidSockId(int nSockId)
{

    return((nSockId >= 1)&&(nSockId <= MAX_PSAM_SOCKET));
}

void QCardReader::SetSockInited(int nSockId, bool bInited)
{
    if(IsValidSockId(nSockId))
    {
        m_bPsamInited[nSockId-1] = bInited;
    }
}


bool QCardReader::SamCmd(int nSockID, const quint8 *pCmd, int nCmdLen, quint8 *pReply, int &nRepLen, int &nErrorCode, char *szErrMsg, int nMaxErrMsgLen)
{
    int nPsamRlt =0;
    quint8 *p;
    char msg1[768];
    char msg2[768];
    nErrorCode=SamCommand(nSockID,pCmd,nCmdLen,pReply,nRepLen);
    memset(msg1,0,sizeof(msg1));
    memset(msg2,0,sizeof(msg2));
    Raw2HexStr(msg1,pCmd,qMin((quint32)nCmdLen,sizeof(msg1)/2-1));
    Raw2HexStr(msg2,pReply,qMin((quint32)nRepLen,sizeof(msg2)/2-1));
    LogMsg("lane",tr("PSAM卡输入：[%1],输出：[%2]").arg(msg1).arg(msg2));
    if(nErrorCode != 0)
    {
        if(szErrMsg && nMaxErrMsgLen > 1)
        {
            GetReaderLastError(szErrMsg,nMaxErrMsgLen);
        }
        return false;
    }
    if(nRepLen < 2)
    {
        nErrorCode = -1;
        if(szErrMsg && nMaxErrMsgLen > 1)
        {
            szErrMsg="返回长度不足";
        }
        return false;
    }
    p = pReply;
    p = p+(nRepLen-2);
    nPsamRlt=(*p)<<8;
    p = p+1;
    nPsamRlt = nPsamRlt + *p;
    if(nPsamRlt != 0x9000)
    {
        nErrorCode = nPsamRlt;
        if(szErrMsg && nMaxErrMsgLen>1)
        {
            QString sTmp = tr("0x%1").arg(nPsamRlt, 4, 16, QLatin1Char('0'));
            memcpy(szErrMsg, sTmp.toLocal8Bit().data(), sTmp.length());
        }
        return false;
    }
    return true;

}

int QCardReader::SamCommand(int iSockID, const quint8 *sCommand, int iLenCmd, quint8* sReply, int &iLenRep)
{
    if(( !m_bDriverLoaded)||(JT_SamCommand==NULL))
    {
        return -1;

    }
    return JT_SamCommand(m_nComID,iSockID,sCommand,iLenCmd,sReply,iLenRep);
}

bool QCardReader::GetIsNewMifareOneCard()
{
    return GetIsNewMifareOneCard((char*)m_Sector00);

}
bool QCardReader::GetIsNewMifareOneCard(const char *SectorData)
{
    //0扇区第2块以0F 01 开头的为新卡！
    return( ((SectorData[16] == 0x0F)&&(SectorData[17] == 0x01)));
}
bool QCardReader::GetIsNewProCard()
{
    return GetIsNewProCard(m_Pro0015Raw);
}

bool QCardReader::GetIsNewProCard(const TPro0015Raw &Pro0015Raw)
{

    //    quint16 wNetId = Bcd2Word(Pro0015Raw.Network);
    quint16 wNetId = bcd2dword((char*)&Pro0015Raw.Network,2);
    return wNetId==ORG_NETWORKID;
}

bool QCardReader::WriteNewOpCard(TOpCardBasicInfo &OpCardBasicInfo,
                                 TOpCardAppInfo &OpCardAppInfo,
                                 bool opWrite2001)
{
    TCardFileConverter sTmp;
    char* szSelectAdfCmd="00A4000002DF02";
    int nSockId = 0,nCmdLen = 0,nReplyLen = 0,nRlt = 0;
    quint8 sCmd[10];
    quint8 sReply[256];
    char szMsg[128];
    TOpCardBasicInfoRaw OpCardBasicInfoRaw;  //1001文件
    TOpCardInfoRaw OpCardInfoRaw;            //1002文件
    TOpCardAppInfoRaw OpCardAppInfoRaw;
    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen = (strlen(szSelectAdfCmd))/ 2;
    Hex2Raw(sCmd,szSelectAdfCmd,strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,sReply, nReplyLen, nRlt, szMsg, 128))
    {
        LogMsg("lane",tr("算密钥错误"));
        return false;
    }


    memset(&OpCardAppInfoRaw, 0, sizeof(OpCardAppInfoRaw));
    memset(&OpCardInfoRaw, 0,sizeof(OpCardInfoRaw));
    m_OpWriteCard = true;
    if(!sTmp.OpCardBasicInfoO2CardBasicInfoRaw(OpCardInfoRaw,
                                               OpCardBasicInfo))
    {
        return false;
    }
    if(!sTmp.OpCardAppInfo2OpCardAppInfoRaw(OpCardAppInfoRaw,OpCardAppInfo))
    {
        return false;
    }

    if (opWrite2001)
    {
        nRlt = JT_WriteFile(m_nComID, 0x2001,KEYB,0, 3, (quint8 *)&OpCardInfoRaw);
        if (nRlt != 0)
        {
            LogMsg("lane",tr("写身份卡2001文件失败"));
            return false;
        }
        else
        {
            LogMsg("lane",tr("写身份卡2001文件成功"));
        }
    }
    nRlt = JT_WriteFile(m_nComID, 0x3001,KEYB,0, 3, (quint8 *)&OpCardAppInfoRaw);
    if (nRlt!=0 )
    {
        LogMsg("lane",tr("写身份卡3001文件失败"));
        return false;
    }
    else
    {
        LogMsg("lane",tr("写身份卡3001文件成功"));
    }
    m_OpWriteCard= false;

    AudioControl(2,1);

    return true;
}
bool QCardReader::WriteNewOpCard(const  TOpCardBasicInfo &OpCardBasicInfo)
{
    TCardFileConverter sTmp;
    char* szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10];
    quint8 sReply[256];
    char szMsg[128];
    TOpCardBasicInfoRaw OpCardBasicInfoRaw;  //1001文件
    TOpCardInfoRaw OpCardInfoRaw;            //2001文件

    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen = strlen(szSelectAdfCmd)/ 2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,sReply, nReplyLen,
                nRlt, szMsg, 128))
    {
        LogMsg("lane",tr("计算密钥错误"));
        return false;
    }

    memset(&OpCardInfoRaw, 0,sizeof(OpCardInfoRaw));
    m_OpWriteCard = true;
    if(!sTmp.OpCardBasicInfoO2CardBasicInfoRaw(OpCardInfoRaw,
                                               OpCardBasicInfo))
    {
        return false;
    }
    nRlt= JT_WriteFile(m_nComID, 0x2001,KEYB,0, 3,(quint8*)&OpCardInfoRaw);
    if (nRlt!=0 )
    {
        LogMsg("lane",tr("写身份卡2001文件失败"));
        return false;
    }
    else
    {
        LogMsg("lane",tr("写身份卡2001文件成功"));
    }
    m_OpWriteCard = false;
    AudioControl(2,1);
    return true;
}

int QCardReader::AudioControl(quint8 cTimes, quint8 cVoice)
{
    if((!m_bDriverLoaded)||(JT_AudioControl == NULL))
    {
        return -1;
    }
    return JT_AudioControl(m_nComID,cTimes,cVoice);
}

bool QCardReader::CalNewCardKey(quint8 blockNo, int sKeyType, int iSockId, quint8 *sReply, int &length)
{
    return true;
}

bool QCardReader::ExternalAuthorize(int nSockId)
{
    const int MAX_LEN=256;

  const char *szSelectAdfCmd1 = "00B095002B";
    //Des初始化

   const char *szSelectAdfCmd2 = "801a480110";
    int nCmeLen = 0,nReplyLen = 0,nRet = 0;
    quint8 sCmd[MAX_LEN];
    quint8 sReply[MAX_LEN];
    char szMsg[MAX_LEN];
    quint32 sRand;

    if(!SelPsamCardFile(nSockId,0xDF01))
    {
        return false;
    }
    if(!SelProCardFile(0x1001))
    {
        return false;
    }
    nReplyLen=16;
    memset(sCmd,0,sizeof(sCmd));
    memset(sReply,0,sizeof(sReply));
    nCmeLen = strlen(szSelectAdfCmd1)/2;
    Hex2Raw(sCmd,szSelectAdfCmd1,strlen(szSelectAdfCmd1));
    if(!ProCmd(sCmd,nCmeLen,sReply,nReplyLen,nRet,szMsg,128))
        return false;
    //取随机数
    sRand=0;
    if(!ProGenRandom(sRand))
        return false;
    //Des初始化
    Hex2Raw(sCmd,szSelectAdfCmd2,strlen(szSelectAdfCmd2));
   //TODO
    memcpy(sCmd + 5,m_ProCardNo,8);
    memcpy(sCmd +13,m_ProCardIssueId,8);
    if(!SamCmd(nSockId,sCmd,21,sReply,nReplyLen,nRet,szMsg,128))
        return false;
    //Des随机数
    sCmd[0]=0x80;
    sCmd[1]=0xFA;
    sCmd[2]=0x00;
    sCmd[3]=0x00;
    sCmd[4]=0x08;
    memcpy(sCmd +5,&sRand,sizeof(sRand));
    sCmd[9]=0x00;
    sCmd[10]=0x00;
    sCmd[11]=0x00;
    sCmd[12]=0x00;
    if(!SamCmd(nSockId,sCmd,13,sReply,nReplyLen,nRet,szMsg,128))
        return false;
    //认证
    sCmd[0]=0x80;
    sCmd[1]=0x82;
    sCmd[2]=0x00;
    sCmd[3]=0x01;
    sCmd[4]=0x08;
    //TODO
    memcpy(sCmd +5,sReply,8);
    memset(sReply,0,sizeof(sReply));
    if(!ProCmd(sCmd,13,sReply,nReplyLen,nRet,szMsg,128))
        return false;
    return true;

}

bool QCardReader::SelPsamCardFile(int nSockId, quint16 wFile)
{
    const int MAX_LEN=128,
            MAX_MSG_LEN=128;
    quint8 Cmd[MAX_LEN],sReply[MAX_LEN];
    char szMsg[MAX_MSG_LEN];

    int  nCmdLen = 0, nReplyLen = 0, nRlt = 0;
    bool bRlt = false;

    memset(Cmd, 0, MAX_LEN);
    //  0 1 2 3 4 5 6
    // 00A4000002XXXX
    Cmd[1] = 0xA4;
    Cmd[4] = 0x02;
    Cmd[5] = qHiByte(wFile);
    Cmd[6] = qLoByte(wFile);
    nCmdLen= 7;
    nReplyLen = MAX_LEN;

    bRlt = SamCmd(nSockId, Cmd, nCmdLen, sReply, nReplyLen, nRlt, szMsg, MAX_MSG_LEN-1);
    if (!bRlt)
    {
        LogMsg("lane",tr("卡读写器[%1]选择PSAM卡[%2]目录失败:%3,Reader Errot Code = %4").arg(m_nReaderId).arg(nSockId).arg(szMsg).arg(nRlt));
    }
    return bRlt;
}

bool QCardReader::SelProCardFile(quint16 wFile)
{
    const int MAX_LEN=128,MAX_MSG_LEN=128;
    quint8 Cmd[MAX_LEN],sReply[MAX_LEN];
    char szMsg[MAX_MSG_LEN];
    bool bRlt = false;
    int nCmdLen = 0, nReplyLen = 0, nRlt = 0;

    memset(Cmd,0,MAX_LEN);
    //  0 1 2 3 4 5 6
    // 00A4000002XXXX
    Cmd[1] = 0xA4;
    Cmd[4] = 0x02;
    Cmd[5] = qHiByte(wFile);
    Cmd[6] = qLoByte(wFile);
    nCmdLen = 7;
    nReplyLen = MAX_LEN;

    bRlt = ProCmd(Cmd, nCmdLen, sReply, nReplyLen, nRlt, szMsg, MAX_MSG_LEN-1);
    if (!bRlt)
    {
        LogMsg("lane",tr("卡读写器[%1]选择PRO卡目录失败").arg(m_nReaderId));
    }
    return bRlt;

}

int QCardReader::ChangeSamBaudrate(int iSockID, int iBaudrate, int iProtocolType)
{
    if((! m_bDriverLoaded)||(JT_ChangeSamBaudrate==NULL))
    {
        return -1;
    }
    return JT_ChangeSamBaudrate(m_nComID,iSockID,iBaudrate,iProtocolType);
}

int QCardReader::CloseCard()
{
    if((!m_bDriverLoaded)||(JT_CloseCard==NULL))
    {
        return -1;
    }
    return JT_CloseCard(m_nComID);
}

int QCardReader::CPUCCommand(const quint8 *sCommand, int iLenCmd, quint8 *sReply, int &iLenRep)
{
    if((!m_bDriverLoaded)||(JT_CPUCCommand==NULL))
    {
        return -1;
    }
    return JT_CPUCCommand(m_nComID,sCommand,iLenCmd,sReply,iLenRep);
}

bool QCardReader::GetCardFileSectorInfo(int nFileID, int &nStartSec, int &nSecCount)
{
    return true;
}

QString QCardReader::GetCardId()
{
    return m_sLastCardId;
}

int QCardReader::GetCardType()
{
    return m_nLastCardType;
}

bool QCardReader::GetIsMifareOneCard(CARD_TYPE_PHYSICAL CardType)
{
    return (CardType==TYPE_M1_S50_BLOCK)||(CardType==TYPE_M1_S50_MAD)||(CardType==TYPE_M1_S70_BLOCK)||(CardType==TYPE_M1_S70_MAD);
}

bool QCardReader::GetPsamTermNo(int nSockID, quint8 *psamTermNo)
{
    if((psamTermNo!=NULL)&&((nSockID>2)||(nSockID<1)))
    {
        memcpy(psamTermNo,m_PsamTermNo[nSockID - 1],sizeof(m_PsamTermNo));
    }
    return true;

}

bool QCardReader::ProCmd(const quint8 *pCmd, int nCmdLen, quint8 *pReply, int &nRepLen, int &nErrorCode, char *szErrMsg, int nMaxErrMsgLen)
{
    quint8 * p;
    int nProCardRlt;
    char msg1[768],msg2[768];
    nErrorCode= CPUCCommand(pCmd, nCmdLen, pReply, nRepLen);

    memset(msg1,0,sizeof(msg1));
    memset(msg2,0,sizeof(msg2));
    Raw2HexStr(msg1,pCmd,qMin((quint32)nCmdLen,sizeof(msg1)/2-1));
    Raw2HexStr(msg2,pReply,qMin((quint32)nRepLen,sizeof(msg2)/2-1));
    LogMsg("lane",tr("PSAM卡输入：[%1],输出：[%2]").arg(msg1).arg(msg2));
    if(nErrorCode != 0)
    {
        if(szErrMsg && nMaxErrMsgLen > 1)
        {
            GetReaderLastError(szErrMsg,nMaxErrMsgLen);
        }
        LogMsg("lane",tr("CPUCmd返回错误代码：[%1]错误：[%2]").arg(nErrorCode).arg(szErrMsg));
        return false;
    }
    if(nRepLen<2)
    {
        nErrorCode=-1;
        if(szErrMsg && nMaxErrMsgLen>1)
        {
           szErrMsg="返回长度不足";

        }
        return false;
    }
    p=pReply;
    nProCardRlt=(*p)<<8;
    p=p+1;
    nProCardRlt = nProCardRlt + *p;
    if(nProCardRlt!=0x9000)
    {
        nErrorCode=nProCardRlt;
        if(szErrMsg && nMaxErrMsgLen>1)
        {
            QString sTmp = tr("0x%1").arg(nProCardRlt, 4, 16, QLatin1Char('0'));
            memcpy(szErrMsg, sTmp.toLocal8Bit().data(), sTmp.length());

        }
        return false;
    }

    return true;
}

bool QCardReader::ProGenRandom(quint32 &dwRandom)
{
    const int MAX_LEN=256;
    char *szSelectAdfCmd="0084000004";

    int nCmdLen, nReplyLen,nRet;
    char szMsg[MAX_LEN];
    quint8 sCmd[MAX_LEN],sReply[MAX_LEN];
    //取随机数
    memset(sCmd,0, sizeof(sCmd));
    memset(sReply,0,sizeof(sReply));
    nReplyLen= 16;

    nCmdLen= strlen(szSelectAdfCmd)/2;
    Hex2Raw(sCmd, szSelectAdfCmd,strlen(szSelectAdfCmd));
    if (!ProCmd(sCmd, nCmdLen, sReply, nReplyLen, nRet, szMsg, 128))
    {
        return false;
    }
    memcpy(&dwRandom,sReply + 2,4);
    return true;
}

bool QCardReader::GetSockInited(int nSockId)
{
    if(IsValidSockId(nSockId))
        return m_bPsamInited[nSockId-1];
    return false;
}

int QCardReader::LEDDisplay(quint8 cRed, quint8 cGreen, quint8 cBlue)
{
    if((!m_bDriverLoaded)||(JT_LEDDisplay==NULL))
        return -1;
    return JT_LEDDisplay(m_nComID,cRed,cGreen,cBlue);
}

bool QCardReader::OpenCard()
{
    QMutexLocker locker(&m_mutex);
    const int MAX_LEN = 1024;
    quint8 buf[MAX_LEN];
    int nCardSerialIdLen, nCardType;
    quint32 dwCardSerialId = 0;
    memset(buf,0,MAX_LEN);
    nCardSerialIdLen = MAX_LEN;
    nCardType=OpenCard(buf,nCardSerialIdLen);
    if((nCardType == 0)||(nCardSerialIdLen < 4))
    {
        //无卡或读卡序列号错误
        return false;
    }
    memcpy(&dwCardSerialId,buf,4);
    dwCardSerialId = qFromBigEndian(dwCardSerialId);
    if((nCardType == TYPE_M1_S50_BLOCK)||(nCardType == TYPE_M1_S50_MAD))
    {
        //M1卡
        if(!ReadPubSector(dwCardSerialId))
        {
            //读未加密扇区错误，可能是系统外卡片
            LogMsg("lane",tr("读未加密扇区错误，可能是系统外卡片"));
            return false;
        }
        SaveOpenCardInfo(nCardType,dwCardSerialId);
        return true;
    }
    else if(nCardType == TYPE_PRO)
    {
        //先保存，下面的read0015需要使用保存后结果
        SaveOpenCardInfo(nCardType,dwCardSerialId);
        //CPU卡
        if(!Read0015(dwCardSerialId))
        {
            //读0015文件，以判断是否是新卡
            LogMsg("lane",tr("读0015文件，以判断是否是新卡"));
            return false;
        }
        return true;

    }
    else
    {
        //暂不支持的卡类型
        LogMsg("lane",tr("暂不支持的卡类型%1").arg(nCardType));
        return false;
    }
    return false;

}

int QCardReader::OpenCard(unsigned char *pCardSer,int &nMaxLength)
{
    if((!m_bDriverLoaded)||(JT_OpenCard==NULL))
    {
        return -1;
    }
    return JT_OpenCard(m_nComID,pCardSer,nMaxLength);
}

bool QCardReader::ProCappForOldCard(int nSockID, TProCardConsumeInfo &ConsumeInfo, quint32 dwMoney, const QDateTime &ConsumeTime, const quint8 *pData)
{
    const int MAX_LEN=128;
    char szMsg[MAX_LEN];
    quint8 sCmd[MAX_LEN];
    quint8 sReply[MAX_LEN];
    bool bRlt = false;
    quint32 dwTmp;
    int nReplyLen, nRlt;
    // 扣款前余额
    quint32 dwOldBalance_BigEndian;
    // 提取用户卡交易序号
    quint16 wTmpCardSeq_BigEndian;
    // 伪随机数
    quint32 dwRandom_BigEndian;
    //交易金额
    quint32 dwMoney_BigEndian;
    // 提取终端交易序号
    quint32   dwTmpTermSeq_BigEndian ;
    //消费密钥版本号
    quint8 bTmpPurKeyVer;
    //算法标识
    quint8 bAlgorithmFlag;
    quint32 dwMac1_BigEndian;
    quint32 dwTac_BigEndian;
    quint32 dwMac2_BigEndian;
    quint8 bcdTime[7];
    ConsumeInfo.dwMoney = dwMoney;
    if(!GetSockInited(nSockID))
    {
        //TODO return -1;
        return false;
    }
    if(!SelPsamCardFile(nSockID,0xDF01))
    {
        return false;
    }
    if(!SelProCardFile(0x1001))
    {
        return false;
    }
    // initialize for capp purchase
    memset(sCmd,0,MAX_LEN);
    sCmd[0]=0x80;
    sCmd[1]=0x50;
    sCmd[2]=0x03;
    sCmd[3]=0x02;
    sCmd[4]=0x0B;
    sCmd[5]=0x01;   //密钥标识
    dwTmp = qFromBigEndian(dwMoney);
    memcpy(sCmd + 6,&dwTmp,4);//交易金额
    memcpy(sCmd + 10,m_PsamTermNo[nSockID - 1],6);
    sCmd[16]=0x0F;
    nReplyLen = MAX_LEN;
    bRlt=ProCmd(sCmd,17,sReply,nReplyLen,nRlt,szMsg,sizeof(szMsg));
    if(!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }
    memcpy(ConsumeInfo.psamTermNo,m_PsamTermNo[nSockID - 1],sizeof(ConsumeInfo.psamTermNo));
    //扣款余额 ‘元’
    memcpy(&dwOldBalance_BigEndian,sReply + 2,4);
    ConsumeInfo.dwBalanceBefore = qFromBigEndian(dwOldBalance_BigEndian);
    ConsumeInfo.dwBalanceAfter=0xFFFFFFFF;
    // 用户卡交易序号(2字节)
    memcpy(&wTmpCardSeq_BigEndian,sReply + 6,2);
    // 3字节透支限额
    // 1字节密钥版本号
    bTmpPurKeyVer=sReply[11];
    // 1字节算法标识
    bAlgorithmFlag=sReply[12];
    memcpy(&dwRandom_BigEndian,sReply + 13,4);
    ConsumeInfo.wCardSeq=qFromBigEndian(wTmpCardSeq_BigEndian);
    // ICC复合交易写0019文件
    sCmd[0]= 0x80;
    sCmd[1]= 0xDC;
    sCmd[2]= 0xAA;
    sCmd[3]= 0xC8;
    sCmd[4]= 0x2B;
    sCmd[5]= 0xAA;
    sCmd[6]= 0x29;
    sCmd[7]= 0x00;
    memcpy(sCmd + 8,pData,FILE_LEN_0012_GB);
    nReplyLen=MAX_LEN;
    bRlt=ProCmd(sCmd,8+FILE_LEN_0012_GB,sReply,nReplyLen,nRlt,szMsg,sizeof(szMsg));
    if(!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }
    if(!systime2bcd((char*)bcdTime,7,ConsumeTime))
    {
        return false;
    }
    dwMoney_BigEndian=qFromBigEndian(dwMoney);
    //旧CPU卡使用一级分散机制
    // init sam for purchase
    sCmd[0]= 0x80;
    sCmd[1]= 0x70;
    sCmd[2]= 0x00;
    sCmd[3]= 0x00;
    sCmd[4]= 0x1C;

    memcpy(sCmd + 5,&dwRandom_BigEndian,4);
    memcpy(sCmd + 9,&wTmpCardSeq_BigEndian,2);
    memcpy(sCmd +11,&dwMoney_BigEndian,4);
    m_bLastTransType=0x09;			//交易类型
    sCmd[15]= m_bLastTransType;
    memcpy(sCmd +16,bcdTime,7);	   //交易时间
    sCmd[23]= bTmpPurKeyVer;    //消费密钥版本号	--需根据卡片返回核对
    sCmd[24]= bAlgorithmFlag;	   //消费密钥算法标识	--需根据卡片返回核对
    memcpy(sCmd + 25,m_ProCardNo,8);	 //用户卡应用序列号
    nReplyLen= MAX_LEN;
    bRlt= SamCmd(nSockID,sCmd, 33, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }
    // 提取终端交易序号
    memcpy(&dwTmpTermSeq_BigEndian,sReply ,4);
    // MAC1
    memcpy(&dwMac1_BigEndian,sReply + 4, 4);
    ConsumeInfo.dwTermSeq= qFromBigEndian(dwTmpTermSeq_BigEndian);
    ConsumeInfo.bTransType= m_bLastTransType;
    ConsumeInfo.bKeyVer= 1;

    // 保存用户卡交易序号和终端交易序号，以备扣款失败使用
    m_wLastConsumeCardSeq= ConsumeInfo.wCardSeq;
    m_dwLastConsumeTermSeq= ConsumeInfo.dwTermSeq;

    // purchase
    sCmd[0]= 0x80;
    sCmd[1] = 0x54;
    sCmd[2] = 0x01;
    sCmd[3] =0x00;
    sCmd[4] = 0x0F;
    memcpy(sCmd + 5,&dwTmpTermSeq_BigEndian,4);
    memcpy(sCmd + 9,bcdTime, 7);
    memcpy(sCmd + 16,&dwMac1_BigEndian,4);
    sCmd[20]= 0x08;
    nReplyLen= MAX_LEN;
    bRlt= ProCmd(sCmd, 21, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }

    // get Tac
//TODO    memcpy(&dwTac_BigEndian,sReply + 2, 4);
     memcpy(&dwTac_BigEndian,sReply, 4);
    // get Mac2
    memcpy(&dwMac2_BigEndian,sReply + 6, 4);
    // 提取Tac
    ConsumeInfo.dwTac= qFromBigEndian(dwTac_BigEndian);
    // 此时扣款已成功
    //旧卡 金额 是"元" 需要转成"分"
    ConsumeInfo.dwBalanceAfter= ConsumeInfo.dwBalanceBefore-dwMoney;
    //扣款前余额 由 '元' 转 '分'
    ConsumeInfo.dwBalanceBefore = ConsumeInfo.dwBalanceBefore * 100;
    ConsumeInfo.dwBalanceAfter= ConsumeInfo.dwBalanceAfter * 100;

    // check Mac2
    sCmd[0] = 0x80;
    sCmd[1] = 0x72;
    sCmd[2] = 0x00;
    sCmd[3] = 0x00;
    sCmd[4] = 0x04;
    memcpy(sCmd + 5,&dwMac2_BigEndian,4);
    nReplyLen = MAX_LEN;
    bRlt = SamCmd(nSockID, sCmd, 9, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("忽略check Mac2错误"));
    }
    return true;
}

bool QCardReader::ProCapp(int nSockID, TProCardConsumeInfo &ConsumeInfo, quint32 dwMoney, const QDateTime &ConsumeTime,  quint8 *pData)
{
    const int MAX_LEN=128;
    char szMsg[MAX_LEN];
    quint8 sCmd[MAX_LEN];
    quint8 sReply[MAX_LEN];
    bool bRlt;
    quint32 dwTmp;
    int nReplyLen, nRlt;
    // 扣款前余额
    quint32 dwOldBalance_BigEndian;
    // 提取用户卡交易序号
    quint16 wTmpCardSeq_BigEndian;
    // 伪随机数
    quint32 dwRandom_BigEndian;
    //交易金额
    quint32 dwMoney_BigEndian;
    // 提取终端交易序号
    quint32   dwTmpTermSeq_BigEndian ;
    //消费密钥版本号
    quint8 bTmpPurKeyVer;
    //算法标识
    quint8 bAlgorithmFlag;
    quint32 dwMac1_BigEndian;
    quint32 dwTac_BigEndian;
    quint32 dwMac2_BigEndian;
    quint8 bcdTime[7];
    ConsumeInfo.dwMoney = dwMoney;
    if(!GetSockInited(nSockID))
    {
        return false;
    }
    // 国标卡的密钥在DF01; 前面可能因通行卡的密钥在DF02而选择了其它应用
    if(!SelPsamCardFile(nSockID,0xDF01))
    {
        return false;
    }
    // 为防止卡片被中途拿走后再放回造成写卡失败，重新选择一次应用目录
    if(!SelProCardFile(0x1001))
    {
        return false;
    }
    // initialize for capp purchase
    memset(sCmd,0,MAX_LEN);
    sCmd[0]=0x80;
    sCmd[1]=0x50;
    sCmd[2]=0x03;
    sCmd[3]=0x02;
    sCmd[4]=0x0B;
    sCmd[5]=0x01;   //密钥标识
    dwTmp = qFromBigEndian(dwMoney);
    memcpy(sCmd + 6,&dwTmp,4);//交易金额
    memcpy(sCmd + 10,m_PsamTermNo[nSockID - 1],6);
    sCmd[16]=0x0F;
    nReplyLen=MAX_LEN;
    bRlt=ProCmd(sCmd,17,sReply,nReplyLen,nRlt,szMsg,sizeof(szMsg));
    if(!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }
    memcpy(ConsumeInfo.psamTermNo,m_PsamTermNo[nSockID - 1],sizeof(ConsumeInfo.psamTermNo));
    //扣款余额 ‘元’
    memcpy(&dwOldBalance_BigEndian,sReply + 2,4);
    ConsumeInfo.dwBalanceBefore = qFromBigEndian(dwOldBalance_BigEndian);
    ConsumeInfo.dwBalanceAfter = 0xFFFFFFFF;
    // 用户卡交易序号(2字节)
    memcpy(&wTmpCardSeq_BigEndian,sReply + 6,2);
    // 3字节透支限额
    // 1字节密钥版本号
    bTmpPurKeyVer = sReply[11];
    // 1字节算法标识
    bAlgorithmFlag = sReply[12];
    memcpy(&dwRandom_BigEndian,sReply + 13,4);
    ConsumeInfo.wCardSeq=qFromBigEndian(wTmpCardSeq_BigEndian);
    // ICC复合交易写0019文件
    sCmd[0]= 0x80;
    sCmd[1]= 0xDC;
    sCmd[2]= 0xAA;
    sCmd[3]= 0xC8;
    sCmd[4]= 0x2B;
    sCmd[5]= 0xAA;
    sCmd[6]= 0x29;
    sCmd[7]= 0x00;
    memcpy(sCmd + 8,pData,FILE_LEN_0012_GB);
    nReplyLen=MAX_LEN;
    bRlt=ProCmd(sCmd,8+FILE_LEN_0012_GB,sReply,nReplyLen,nRlt,szMsg,sizeof(szMsg));
    if(!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }
    if(!systime2bcd((char*)bcdTime,7,ConsumeTime))
    {
        return false;
    }
    dwMoney_BigEndian = qFromBigEndian(dwMoney);
    //旧CPU卡使用一级分散机制
    // init sam for purchase
    sCmd[0]= 0x80;
    sCmd[1]= 0x70;
    sCmd[2]= 0x00;
    sCmd[3]= 0x00;
    sCmd[4]= 0x24;
    memcpy(sCmd + 5,&dwRandom_BigEndian,4);
    memcpy(sCmd + 9,&wTmpCardSeq_BigEndian,2);
    memcpy(sCmd + 11,&dwMoney_BigEndian,4);
    m_bLastTransType=0x09;			//交易类型
    sCmd[15]= m_bLastTransType;
    memcpy(sCmd + 16,bcdTime,7);	   //交易时间
    sCmd[23]= bTmpPurKeyVer;    //消费密钥版本号	--需根据卡片返回核对
    sCmd[24]= bAlgorithmFlag;	   //消费密钥算法标识	--需根据卡片返回核对
    memcpy(sCmd + 25, m_ProCardNo,8);	 //用户卡应用序列号
    memcpy(sCmd + 33, m_ProCardIssueId , 8);
    nReplyLen= MAX_LEN;
    bRlt= SamCmd(nSockID,sCmd, 41, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }


    // 提取终端交易序号
    memcpy(&dwTmpTermSeq_BigEndian,sReply ,4);
    // MAC1
    memcpy(&dwMac1_BigEndian,sReply + 4, 4);
    ConsumeInfo.dwTermSeq= qFromBigEndian(dwTmpTermSeq_BigEndian);
    ConsumeInfo.bTransType= m_bLastTransType;
    ConsumeInfo.bKeyVer= 1;

    // 保存用户卡交易序号和终端交易序号，以备扣款失败使用
    m_wLastConsumeCardSeq= ConsumeInfo.wCardSeq;
    m_dwLastConsumeTermSeq= ConsumeInfo.dwTermSeq;

    // purchase
    sCmd[0]= 0x80;
    sCmd[1] = 0x54;
    sCmd[2] = 0x01;
    sCmd[3] =0x00;
    sCmd[4] = 0x0F;
    memcpy(sCmd + 5,&dwTmpTermSeq_BigEndian,4);
    memcpy(sCmd + 9,bcdTime, 7);
    memcpy(sCmd + 16,&dwMac1_BigEndian,4);
    sCmd[20]= 0x08;
    nReplyLen= MAX_LEN;
    bRlt= ProCmd(sCmd, 21, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("错误信息[%1]").arg(szMsg));
        return false;
    }

    // get Tac
    memcpy(&dwTac_BigEndian,sReply + 2, 4);
    // get Mac2
    memcpy(&dwMac2_BigEndian,sReply + 6, 4);
    // 提取Tac
    ConsumeInfo.dwTac= qFromBigEndian(dwTac_BigEndian);
    // 此时扣款已成功
    //旧卡 金额 是"元" 需要转成"分"
    ConsumeInfo.dwBalanceAfter= ConsumeInfo.dwBalanceBefore-dwMoney;
    //扣款前余额 由 '元' 转 '分'
    ConsumeInfo.dwBalanceBefore = ConsumeInfo.dwBalanceBefore * 100;
    ConsumeInfo.dwBalanceAfter= ConsumeInfo.dwBalanceAfter * 100;

    // check Mac2
    sCmd[0] = 0x80;
    sCmd[1] = 0x72;
    sCmd[2] = 0x00;
    sCmd[3] = 0x00;
    sCmd[4] = 0x04;
    memcpy(sCmd + 5,&dwMac2_BigEndian,4);
    nReplyLen = MAX_LEN;
    bRlt = SamCmd(nSockID, sCmd, 9, sReply, nReplyLen, nRlt, szMsg, sizeof(szMsg));
    if (!bRlt)
    {
        LogMsg("lane",tr("忽略check Mac2错误"));
    }
    return true;
}

bool QCardReader::ProConsume(QString sCardId, quint32 dwMoney, const QDateTime& ConsumeTime, TProCardConsumeInfo &pConsumeInfo)
{
    //QDebug() << "ProConsume";
    QMutexLocker locker(&m_mutex);

    TPro0012Raw raw0012;
    quint32 dwMoneyYuan;
    //pro卡直接写出口信息：修改入出口状态
    raw0012= m_TollInfoWriteFailureProtector.m_Pro0012Raw;

    if (GetIsNewProCard())
    {
        raw0012.bPassStatus= 0x00;
        if(!ProCapp(NEW_PSAM_SLOT, pConsumeInfo,dwMoney,ConsumeTime,(quint8 *)&raw0012))
        {
            return false;
        }
    }

    else
    {
        raw0012.bPassStatus=0x01;
        dwMoneyYuan= dwMoney/100;
        if(!ProCappForOldCard(OLD_PSAM_SLOT, pConsumeInfo,dwMoneyYuan,ConsumeTime,(quint8*)&raw0012))
        {
            return false;
        }
    }
    AudioControl(1, 1);
    return true;
}

bool QCardReader::ProModifyFile(const quint8 *pModifyCmd, quint16 wCmdLen, const quint8 *Mac2)
{
    const int MAX_LEN=256;
    int nReplyLen,nRet;
    char szMsg[MAX_LEN];
    quint8 sCmd[MAX_LEN],sReply[MAX_LEN];
    quint8 sData[9];
    bool bRet;


    nReplyLen = 16;

    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    memset(sData, 0,sizeof(sData));
    if (wCmdLen > 500 )
    {
        return false;
    }

    // 合并Mac2形成最终的修改指令
    memset(sCmd, 0,sizeof(sCmd));
    memcpy(sCmd,pModifyCmd, wCmdLen);
    memcpy(sCmd,Mac2,4);
    sCmd[wCmdLen+4]= 0;
    sReply[0]=0;

    bRet= ProCmd(sCmd, wCmdLen+4, sReply, nReplyLen, nRet, szMsg, 128);
    if (!bRet)
    {
        return false;
    }

    return true;

}

bool QCardReader::ProTraditionConsume(quint32 dwCardId, quint32 dwMoney, const QDateTime& ConsumeTime, TProCardConsumeInfo &ConsumeInfo)
{
    QMutexLocker locker(&m_mutex);

    const int MAX_LEN = 128;
    quint8  sCmd[MAX_LEN],sReply[MAX_LEN];
    bool    bRlt;
    quint32 dwTmp;
    int nRlt,nReplyLen;
    quint32 dwOldBalance_BigEndian;
    quint16 wTmpCardSeq_BigEndian;
    quint32 dwRandom_BigEndian;
    quint32 dwMoney_BigEndian;
    quint32 dwTmpTermSeq_BigEndian;
    quint32     dwMac1_BigEndian;
    quint32 dwTac_BigEndian, dwMac2_BigEndian;
    quint8  bcdTime[7];
    ConsumeInfo.dwMoney= dwMoney;
    if (!GetSockInited(NEW_PSAM_SLOT))
    {
        return false;
    }
    if (!SelPsamCardFile(NEW_PSAM_SLOT, 0xDF01))
    {
        return false;
    }
    // initialize for purchase
    memset(sCmd, 0, MAX_LEN);
    sCmd[0]= 0x80;
    sCmd[1] = 0x50;
    sCmd[2] = 0x01;
    sCmd[3] = 0x02;
    sCmd[4] = 0x0B;
    sCmd[5] = 0x01;
    dwTmp= qFromBigEndian(dwMoney);
    memcpy(sCmd + 6,&dwTmp,4);
    //TODO
    memcpy(sCmd + 10,m_PsamTermNo[NEW_PSAM_SLOT - 1],6);
    sCmd[16] = 0x0f;
    nReplyLen= MAX_LEN;
    bRlt= ProCmd(sCmd, 17, sReply, nReplyLen, nRlt, NULL, 0);
    if (! bRlt)
    {
        return false;
    }
    memcpy(ConsumeInfo.psamTermNo,m_PsamTermNo[NEW_PSAM_SLOT - 1],sizeof(ConsumeInfo.psamTermNo));

    // 扣款前余额
    memcpy(&dwOldBalance_BigEndian,sReply + 2, 4);
    ConsumeInfo.dwBalanceBefore= qFromBigEndian(dwOldBalance_BigEndian);
    ConsumeInfo.dwBalanceAfter = 0xFFFFFFFF;

    // 提取用户卡交易序号
    // 用户卡交易序号(2字节)
    memcpy(&wTmpCardSeq_BigEndian,sReply + 6, 2);

    memcpy(&dwRandom_BigEndian,sReply + 13,4);
    ConsumeInfo.wCardSeq = qFromBigEndian(wTmpCardSeq_BigEndian);
  //TODO 此处bcdtime是否需要加 &
    if (! systime2bcd((char*)bcdTime, 7, ConsumeTime))
    {
        return false;
    }
    dwMoney_BigEndian= qFromBigEndian(dwMoney);

    sCmd[0] = 0x80;
    sCmd[1] = 0x70;
    sCmd[2] = 0x00;
    sCmd[3] = 0x00;
    sCmd[4] = 0x24;
    memcpy(sCmd + 5,&dwRandom_BigEndian, 4);
    memcpy(sCmd + 9,&wTmpCardSeq_BigEndian, 2);
    memcpy(sCmd + 11,&dwMoney_BigEndian, 4);
    m_bLastTransType = 0x06;
    sCmd[15]= m_bLastTransType;
    memcpy(sCmd + 16,bcdTime, 7);
    sCmd[23] = 0x01;							//消费密钥版本号	--需根据卡片返回核对
    sCmd[24] = 0x00;							//消费密钥算法标识	--需根据卡片返回核对
    memcpy(sCmd + 25,m_ProCardNo,  8);		//用户卡应用序列号
    memcpy(sCmd + 33,m_ProCardIssueId,8);
    nReplyLen = MAX_LEN;
    bRlt = SamCmd(NEW_PSAM_SLOT, sCmd, 41, sReply, nReplyLen, nRlt, NULL, 0);
    if (! bRlt)
    {
        return false;
    }


    // 提取终端交易序号

    memcpy(&dwTmpTermSeq_BigEndian,sReply,4);
    // MAC1
    memcpy(&dwMac1_BigEndian,sReply + 4, 4);
    ConsumeInfo.dwTermSeq= qFromBigEndian(dwTmpTermSeq_BigEndian);
    ConsumeInfo.bTransType = m_bLastTransType;
    ConsumeInfo.bKeyVer = 1;

    // 保存用户卡交易序号和终端交易序号，以备扣款失败使用
    m_wLastConsumeCardSeq = ConsumeInfo.wCardSeq;
    m_dwLastConsumeTermSeq = ConsumeInfo.dwTermSeq;

    // purchase
    memset(sCmd, 0,sizeof(sCmd));
    sCmd[0] = 0x80;
    sCmd[1] = 0x54;
    sCmd[2] = 0x01;
    sCmd[3] = 0x00;
    sCmd[4] = 0x0F;
    memcpy(sCmd + 5,&dwTmpTermSeq_BigEndian,  4);
    memcpy(sCmd + 9,bcdTime, 7);
    memcpy(sCmd + 16,&dwMac1_BigEndian,  4);
    sCmd[20] = 0x08;
    nReplyLen = MAX_LEN;
    bRlt = ProCmd(sCmd, 21, sReply, nReplyLen, nRlt, NULL, 0);
    if ( !bRlt )
    {
        return false;
    }


    // get Tac
    memcpy(&dwTac_BigEndian,sReply + 2, 4);
    // get Mac2
    memcpy(&dwMac2_BigEndian,sReply + 6 ,4);
    // 提取Tac
    ConsumeInfo.dwTac = qFromBigEndian(dwTac_BigEndian);
    // 此时扣款已成功
    ConsumeInfo.dwBalanceAfter= (ConsumeInfo.dwBalanceBefore-dwMoney);

    // check Mac2
    sCmd[0] = 0x80;
    sCmd[1]= 0x72;
    sCmd[2] = 0x00;
    sCmd[3] = 0x00;
    sCmd[4] = 0x04;
    memcpy(sCmd + 5,&dwMac2_BigEndian, 4);
    nReplyLen =MAX_LEN ;
    bRlt = SamCmd(NEW_PSAM_SLOT, sCmd, 9, sReply, nReplyLen, nRlt, NULL, 0);
    if (!bRlt)
    {
        LogMsg("lane",tr("忽略check Mac2错误"));
    }

    AudioControl(2, 1);
    return true;
}

int QCardReader::ReadBlock(int iKeyType, int iBlockn, const quint8 *sKey, quint8 *sReply)
{
    char msg[256];
    if ((!m_bDriverLoaded)||(JT_ReadBlock==NULL))
    {
        return -1;
    }
    int nRlt= JT_ReadBlock(m_nComID, iKeyType, iBlockn, sKey, sReply);
    if (nRlt == 0)
    {

        Raw2HexStr(msg,sReply,BLOCK_LEN);
        LogMsg("lane",tr("block %1 : %2").arg(iBlockn).arg(msg));
    }


    return nRlt;
}

bool QCardReader::ReadNewPassTollCard(TTollCardBasicInfo &TollCardBasicInfo, TCardTollInfo &CardTollInfo)
{
    TTollCardBasicInfoRaw TollCardBasicInfoRaw;
    TTollCardTollInfoRaw TollCardTollInfoRaw;
    TCardFileConverter sTmp;
    //po = &TollCardTollInfoRaw;
    int nRlt = 0;
    QString sCardId;
    if (!SelPsamCardFile(NEW_PSAM_SLOT, 0xDF02))
    {
        return false;
    }

    if ((JT_ReadFile(m_nComID,0x1001,KEYA,0,3,(quint8*)&TollCardBasicInfoRaw))!=0)
    {
        return false;
    }
    if (! sTmp.TollCardBasicInfoRaw2TollCardBasicInfo(TollCardBasicInfo,m_dwLastCardId,TollCardBasicInfoRaw))
    {
        return false;
    }
    if (TollCardBasicInfo.M1BasicInfo.bType != CARD_TYPE_PASS_CARD)
    {
        // 卡类型已转换成功，不是通行卡，不必继续处理
        CardTollInfo.clear();
        return false;
    }


    sCardId = QString::number( TollCardBasicInfo.M1BasicInfo.dwCardNum);
    if (m_TollInfoWriteFailureProtector.IsWriteFailedCard(sCardId))
    {
        LogMsg("lane",tr("卡印刷号%1的卡片上次写通行信息失败，使用首次读入的通行信息").arg(sCardId));
        if (!sTmp.TollCardTollInfoRaw2CardTollInfo(CardTollInfo, m_TollInfoWriteFailureProtector.m_TollCardTollInfoRaw))
        {
            return false;
        }
        // NOTE: 卡已经被尝试写过，肯定已通过校验，不再重复校验
        CardTollInfo.bCRC=true;
    }

    else
    {

        nRlt = JT_ReadFile(m_nComID,0x2001,KEYA,0,6,(quint8 *)&TollCardTollInfoRaw);

    }
    if (nRlt !=0)
    {
        return false;
    }

    if (! sTmp.TollCardTollInfoRaw2CardTollInfo(CardTollInfo,TollCardTollInfoRaw))
    {
        return false;
    }

    m_TollInfoWriteFailureProtector.SaveTollInfoForWriteFailure(sCardId, TollCardTollInfoRaw);

    m_dwLastCardNo = TollCardBasicInfo.M1BasicInfo.dwCardNum;
    m_sLastCardId = sCardId;

    return true;

}

bool QCardReader::ReadOpCard(TOpCardBasicInfo &OpCardBasicInfo, TOpCardAppInfo &OpCardAppInfo)
{
    QMutexLocker locker(&m_mutex);
    bool bRlt;

    // 判断是否是M1卡
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        // NOTE: 程序不应走到这一步
        return false;
    }


    //是否是新M1卡
    if (GetIsNewMifareOneCard())
    {

        bRlt= ReadNewOpCard(OpCardBasicInfo, OpCardAppInfo);
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }
    else
    {

        LogMsg("lane",tr("暂不支持老标签卡"));
        bRlt= ReadOldOpCard(OpCardBasicInfo, OpCardAppInfo);
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }

    if (bRlt)
    {

        m_dwLastCardNo= OpCardBasicInfo.M1BasicInfo.dwCardNum;
        m_sLastCardId= QString::number( OpCardBasicInfo.M1BasicInfo.dwCardNum);
    }
    return false;

}

bool QCardReader::ReadProCard(TProCardBasicInfo &ProCardBasicInfo, quint32 &dwBalance, TCardTollInfo &CardTollInfo)
{
    QMutexLocker locker(&m_mutex);

    if (m_nLastCardType!=TYPE_PRO)
    {
        return false;
    }

    if (GetIsNewProCard())
    {
        if (!ReadNewProCard(m_Pro0015Raw, ProCardBasicInfo, dwBalance, CardTollInfo))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool QCardReader::ReadProFile(quint8 bFileId, quint8 bBgnPosition, quint8 bLenToRead, quint8 *pData, quint8 &bLenRead)
{
    int ilen,erroCode,errLen;
    ilen = 600;
    quint8  sReply[600];
    quint8  sCommand[256];
    char  szMsg[128];
    quint8 TmpLen[4];
    quint8* pB;

    errLen = 128;
    if (m_nLastCardType != TYPE_PRO)
    {
        return false;
    }
    memset(sCommand, 0, sizeof(sCommand));
    memset(sReply, 0,sizeof(sReply));
    if (bFileId == 0x19)
    {
        // Read record file.
        sCommand[0] = 0x00;
        sCommand[1] = 0xb2;
        sCommand[2]= 0x01;
        sCommand[3] = 0xCC;
        sCommand[4]= bLenToRead;
    }
    else if (bFileId == 0x18)
    {
        // Read record file.
        sCommand[0] = 0x00;
        sCommand[1] = 0xb2;
        sCommand[2] = 0x01;
        sCommand[3] = 0xC4;
        sCommand[4] = bLenToRead;
    }
    else
    {
        // Read binary file.
        sCommand[0] = 0x00;
        sCommand[1] = 0xb0;
        sCommand[2] = bFileId + 0x80;
        sCommand[3] = bBgnPosition;
        sCommand[4] = bLenToRead;
    }
    if (!ProCmd(sCommand,5,sReply,ilen,erroCode,szMsg,errLen))
    {
        return false;
    }

    if(sReply[0] == 0x6c && bLenToRead == 0)
    {
        //有些卡片不支持使用0去读，所以在返回长度时需要递归下
        //暂时不支持读长度超过0xFF的文件
        TmpLen[0] = 0;
        memcpy(TmpLen,sReply + 1,1);
        if (TmpLen[0] == 0)
        {
            return false;
        }
        return ReadProFile(bFileId, bBgnPosition, TmpLen[0], pData, bLenRead);
    }
    bLenRead = ilen-2;
    memcpy(pData ,sReply + 2 , bLenRead);
    if ((bFileId == 0x15) && (bBgnPosition == 0) &&(bLenRead>=20))
    {
        pB = pData;
        memcpy(m_ProCardIssueId, pData,4);
        memcpy(m_ProCardIssueId + 4, pData,4);
        pB+=12;
        memcpy( m_ProCardNo, pB,8);
    }
    return true;
}
bool QCardReader::ReadPsamTermNo(int nSockId, char *PsamTermNo)
{
    const int MAX_LEN=128;
    const int MAX_MSG_LEN=128;
    quint8 cmd[5]={0x00,0xB0,0x96,0x00,0x06};
    quint8 sReply[MAX_LEN];
    char szMsg[MAX_MSG_LEN];
    int nRlt,nReplyLen;
    bool bRlt;
    memset(szMsg,0,MAX_MSG_LEN);
    bRlt=SamCmd(nSockId,cmd,5,sReply,nReplyLen,nRlt,szMsg,MAX_MSG_LEN);
    if(bRlt && (nReplyLen>=6))
    {
        memcpy(PsamTermNo,sReply,6);
        return true;
    }
    LogMsg("lane",tr("卡读写器[%1]读PSAM卡终端机编号失败").arg(m_nReaderId));
    return false;
}


bool QCardReader::ReadPubSector(quint16 dwCardSerialId)
{
    const int MARGIN_LEN=16;
    quint8 baSector00[SECTOR_LEN+MARGIN_LEN];
    int i;

    if (dwCardSerialId == m_dwLastCardId)
    {
        // 已读过该卡，不必重复读
        return true;

    }

    memset(baSector00, 0,SECTOR_LEN+MARGIN_LEN);

    for(i=0;i<=2;i++)
    {
        if ((ReadBlock(KEYA, i, PUB_KEY, &baSector00[i*BLOCK_LEN]))!= 0)
        {
            return false;

        }
    }
    memcpy(m_Sector00,baSector00,SECTOR_LEN);
    return true;

}

bool QCardReader::ResetPsam(int nSockId, int nInitBaudrate, int nWorkBaudrate, int nProtocolType)
{
    const int MAX_MSG_LEN=128,MAX_LEN=128;
    char szMsg[MAX_MSG_LEN];
    quint8 sReply[MAX_LEN];
    int nReplyLen;
    int nRlt;
    nReplyLen = MAX_LEN;
    // 以初始波特率复位PSAM卡
    nRlt = SamReset(nSockId, nInitBaudrate, nProtocolType, sReply, nReplyLen);
    if (nRlt != 0)
    {
        GetReaderLastError(szMsg, MAX_MSG_LEN-1);
        LogMsg("lane",tr("卡读写器[%1]PSAM卡[%2]复位失败").arg(m_nReaderId).arg(nSockId));

        return false;
    }

    return true;

}

int QCardReader::SamReset(int iSockID, int iBaudrate, int iProtocolType, quint8 *sReply, int &iLenRep)
{
    if(m_bDriverLoaded && JT_SamReset==NULL)
    {
        return -1;
    }
    return JT_SamReset(m_nComID, iSockID, iBaudrate, iProtocolType, sReply, iLenRep);
}

void QCardReader::SaveOpenCardInfo(int nCardType, quint32 dwCardSerialId)
{
    m_nLastCardType = nCardType;
    m_dwLastCardId = dwCardSerialId;
    //清空印刷卡号
    m_sLastCardId.clear();
}

bool QCardReader::WriteBack(QString sCardId)
{
    //读卡时，要将读入的信息保存下来（作为静态变量），
    //写出口信息时只更新入出状态和卡箱号2个字段
    if (m_nLastCardType == TYPE_PRO)
    {
        if (GetIsNewProCard())
        {
            return WriteBack_NewProCard(sCardId);
        }
        else
        {
            return WriteBack_OldProCard(sCardId);
        }
    }
    else
    {
        if (GetIsNewMifareOneCard())
        {
            return WriteBack_NewPassCard(sCardId);
        }
        else
        {
            return WriteBack_OldPassCard(sCardId);
        }
    }
    return false;
}

int QCardReader::WriteBlock(int iKeyType, int iBlockn, const quint8 *sKey, const quint8 *sData)
{
    if((!m_bDriverLoaded)||(JT_WriteBlock==NULL))
    {
        return -1;
    }
    return JT_WriteBlock(m_nComID, iKeyType, iBlockn, sKey, sData);
}

bool QCardReader::WriteCardFile(int iFileID, int iAddr, int iLength, quint8 *sData)
{
    int iStartSec, iSecCount,iBlockn, iBlocks, length, iBlock;
    quint8 sBlock[17];
    quint8  key[6];
    int nRet, i;
    quint8* pB;

    // 检查参数是否正确
    if ((iFileID<0)||(iFileID>15))
    {
        return false;
    }
    if (iAddr<0)
    {
        return false;
    }
    if (iLength<0)
    {
        return false;
    }
    if (sData ==NULL)
    {
        return false;
    }
    if (!GetCardFileSectorInfo(iFileID, iStartSec, iSecCount))
    {
        return false;
    }
    // 起始扇区中的起始块
    iBlockn = iAddr/16;
    // 起始块内偏移地址
    // 总块数
    iBlocks = (iLength+iAddr-1) / 16-iBlockn+1;
    length = 1;
    memset(key, 0,sizeof(key));
    if (iAddr > (iSecCount*3*16-2))
    {
        return false;
    }
    if (iBlocks>iSecCount*3)
    {
        return false;
    }


    iBlock = iStartSec*4+iBlockn;
    for(i=0;i<=iBlocks-1;i++)
    {
        if ((iBlock+1)%4==0)
        {
            iBlock++;
        }

        if(!CalNewCardKey(iBlock, KEYB,2,key,length))
        {
            return false;
        }
        memset(sBlock, 0,sizeof(sBlock));
        pB= sData;
        pB+=16*i;
        memcpy(sBlock,pB, 16);
        sBlock[16]= 0x00;
        nRet = WriteBlock(KEYB,iBlock,key,sBlock);
        if(nRet!= 0)
        {
            if(m_OpWriteCard)
            {
                LogMsg("lane",tr("写身份卡[%1]失败").arg(iBlock));
            }
            return false;
        }
        iBlock++;
    }
    return true;
}

bool QCardReader::WriteEntryInfo(QString sCardId, TCardTollInfo &CardTollInfo)
{
    bool bRlt = false;

    //如果卡序列号与读入保存的不一致，返回写失败
    if (sCardId!=m_sLastCardId)
    {
        LogMsg("lane",tr("写入口失败：读到的卡号[%1]与要写的卡号[%2]不一致").arg(m_sLastCardId).arg(sCardId));
        return false;
    }

    if (m_nLastCardType == TYPE_PRO)
    {
        if (GetIsNewProCard())
        {
            memcpy(CardTollInfo.baPsamTermId, m_PsamTermNo[NEW_PSAM_SLOT -1], 6);
            bRlt= WriteEntryInfo_NewProCard(CardTollInfo);
        }
        else
        {
            memcpy(CardTollInfo.baPsamTermId,m_PsamTermNo[OLD_PSAM_SLOT-1],  6);
            bRlt = WriteEntryInfo_OldProCard(CardTollInfo);
        }
    }
    else
        if (GetIsNewMifareOneCard())
        {

            memcpy(CardTollInfo.baPsamTermId,m_PsamTermNo[NEW_PSAM_SLOT -1],  6);
            bRlt = WriteEntryInfo_NewPassCard(CardTollInfo);
        }
        else
        {
            bRlt = false;
        }



    // 记录是否有写卡失败发生
    if (!bRlt)
    {

        m_TollInfoWriteFailureProtector.MarkWriteFailure(m_sLastCardId);
        LogMsg("lane",tr("写卡失败"));
    }
    else
    {
        m_TollInfoWriteFailureProtector.MarkWriteSuccess(m_sLastCardId);
        LogMsg("lane",tr("写卡成功"));
    }
    return bRlt;
}

bool QCardReader::WriteExitInfo(QString sCardId, quint32 dwBoxNo)
{
    bool bRlt =false;

    if (sCardId !=m_sLastCardId)
    {
        LogMsg("lane",tr("写出口失败：读到的卡号[%1]与要写的卡号[%2]不一致").arg(m_sLastCardId).arg(sCardId));
        return false;
    }

    if (m_nLastCardType ==TYPE_PRO)
    {
        // 非现金卡直接返回，出口信息直接在复合消费里写入
        return false;
    }

    if (GetIsNewMifareOneCard())
    {
        bRlt= WriteExitInfo_NewPassCard(dwBoxNo);
    }
    else
    {
        bRlt = WriteExitInfo_OldPassCard(dwBoxNo);
    }
    // 记录是否有写卡失败发生
    if (!bRlt)
    {
        m_TollInfoWriteFailureProtector.MarkWriteFailure(m_sLastCardId);
        LogMsg("lane",tr("写卡失败"));
    }
    else
    {
        m_TollInfoWriteFailureProtector.MarkWriteSuccess(m_sLastCardId);
        LogMsg("lane",tr("写卡成功"));
    }
    return bRlt;
}

bool QCardReader::WriteOpCardDll(const TDLLOpCardBasicInfo &OpCardBasicInfo)
{
    TCardFileConverter sTmp;
    const char *szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10];
    quint8  sReply[256];
    char szMsg[128];
    TOpCardInfoRaw OpCardInfoRaw ;            //2001文件

    // 判断是否为M1卡
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        LogMsg("lane",tr("非M1卡"));
        return false;
    }

    //是否是新M1卡
    if (!GetIsNewMifareOneCard())
    {
        LogMsg("lane",tr("非新M1卡"));
        return false;
    }

    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen = strlen(szSelectAdfCmd)/2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,sReply, nReplyLen, nRlt, szMsg, 128))
    {
        LogMsg("lane",tr("计算密钥错误"));
        return false;
    }

    memset(&OpCardInfoRaw, 0,sizeof(OpCardInfoRaw));
    m_OpWriteCard = true;
    if(! sTmp.DLLOpCardBasicInfoO2CardBasicInfoRaw(OpCardInfoRaw, OpCardBasicInfo))
    {
        return false;
    }
    nRlt = JT_WriteFile(m_nComID, 0x2001,KEYB,0, 3, (quint8 *)&OpCardInfoRaw);
    if (nRlt!=0 )
    {
        LogMsg("lane",tr("写身份卡2001文件失败"));
        return false;
    }
    else
    {
        LogMsg("lane",tr("写身份卡2001文件成功"));
    }
    m_OpWriteCard= false;
    AudioControl(2,1);
    return true;
}

bool QCardReader::WriteOpCard(TOpCardBasicInfo &OpCardBasicInfo, TOpCardAppInfo &OpCardAppInfo, bool opWrite2001)
{
    bool bRlt = false;

    // 判断
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        LogMsg("lane",tr("非M1卡"));
        return false;
    }

    //是否是新M1卡
    if (GetIsNewMifareOneCard())
    {
        bRlt= WriteNewOpCard(OpCardBasicInfo, OpCardAppInfo, opWrite2001);
        if (bRlt)
        {
            m_dwLastCardNo= OpCardBasicInfo.M1BasicInfo.dwCardNum;
        }
        return bRlt;
    }
    else
    {
        return false;
    }

}

bool QCardReader::WriteTollInfo(const TCardTollInfo &CardTollInfo)
{
    TCardFileConverter sTmp;
    TTollCardTollInfoRaw TollCardTollInfoRaw, writeTollCardTollInfoRaw ;
    quint8 low;
    memset(&TollCardTollInfoRaw, 0,sizeof(TollCardTollInfoRaw));
    memset(&writeTollCardTollInfoRaw, 0,sizeof(writeTollCardTollInfoRaw));
    if((JT_ReadFile(m_nComID,0x2001,KEYA,0,6,(quint8 *)&writeTollCardTollInfoRaw))!=0)
    {
        LogMsg("lane",tr("WriteTollInfo按文件读失败"));
        //       Logger.Error('WriteTollInfo按文件读失败');
        return false;
    }
    if (!sTmp.CardTollInfo2TollCardTollInfoRaw(TollCardTollInfoRaw, CardTollInfo))
    {
        return false;
    }
    // 写通行状态
    low = (TollCardTollInfoRaw.bMaintenanceAndbPassStatus) & 0x0F;
    writeTollCardTollInfoRaw.bMaintenanceAndbPassStatus = writeTollCardTollInfoRaw.bMaintenanceAndbPassStatus & 0xF0 | low;
    // 取卡盒数
    writeTollCardTollInfoRaw.dwCardBoxId = CardTollInfo.dwCardBoxId;
    // 次数加1
    writeTollCardTollInfoRaw.dwWriteCount = int(writeTollCardTollInfoRaw.dwWriteCount)+1;
    if((JT_WriteFile(m_nComID,0x2001,KEYB,0,6,(quint8 *)&writeTollCardTollInfoRaw))!=0)
    {
        return false;
    }
    return true;
}

bool QCardReader::ReadTollCard(TTollCardBasicInfo &TollCardBasicInfo, TCardTollInfo &CardTollInfo)
{
    QMutexLocker locker(&m_mutex);

    bool bRlt = false;

    if (m_nLastCardType ==TYPE_PRO)
    {
        return false;
    }
    else if (GetIsNewMifareOneCard())
    {
        bRlt= ReadNewPassTollCard(TollCardBasicInfo, CardTollInfo);
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }
    else
    {
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }
    return false;
}

bool QCardReader::IsMifareOneCard()
{
    return GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType));
}

bool QCardReader::InitNewPsam(int nSockId)
{
    int nInitBaudrate, nWorkBaudrate, nProtocolType;
    if (!IsValidSockId(nSockId))
    {
        return false;
    }
    SetSockInited(nSockId, false);
    // 复位PSAM卡，起始波特率为9600
    // 复位成功后对于JKM115，将波特率改为38400
    // 复位成功后对于JKR135，将波特率改为115200
    nInitBaudrate = 1;
    nWorkBaudrate = 6;
    nProtocolType = 0;
    if (!ResetPsam(nSockId, nInitBaudrate, nWorkBaudrate, nProtocolType))
    {
        return false;
    }
    // 读取终端机编号
    if (!ReadPsamTermNo(nSockId, (char *)m_PsamTermNo[nSockId - 1]))
    {
        return false;
    }
    SetSockInited(nSockId, true);
    return true;
}
bool QCardReader::InitOldPsam(int nSockId)
{
    int nInitBaudrate,nWorkBaudrate,nProtocolType;
    if(!IsValidSockId(nSockId))
    {
        return false;
    }
    SetSockInited(nSockId,false);
    // 复位老PSAM卡，对应通讯波特率为38400
    nInitBaudrate=2;
    nWorkBaudrate=2;
    nProtocolType=0;
    if(!ReadPsamTermNo(nSockId,(char *)m_PsamTermNo[nSockId - 1]))
    {
        return false;
    }
    SetSockInited(nSockId,true);
    return true;

}
bool QCardReader::WriteProBinFile(int nSockId, quint8 nFileId, bool bNeedExternalAuthorize, quint8 nBgnPosition, const quint8 *pData, int nDataLen)
{
    quint8 sReply[256],sCmd[512];
    char szMsg[128];
    int nReplyLen = 0, nRet = 0;

    if (m_nLastCardType!=TYPE_PRO)
    {
        return false;
    }
    if (nDataLen>255)
    {
        return false;
    }
    memset(sCmd, 0, sizeof(sCmd));
    memset(sReply, 0, sizeof(sReply));
    if (bNeedExternalAuthorize)
    {
        if (!ExternalAuthorize(nSockId))
        {
            return false;
        }
    }
    sCmd[0] = 0x00;
    sCmd[1] = 0xD6;
    sCmd[2] = nFileId+0x80;
    sCmd[3] = nBgnPosition;
    sCmd[4] = nDataLen;
    memcpy(sCmd + 5,pData,nDataLen);
    if (!ProCmd(sCmd, 5+nDataLen, sReply, nReplyLen, nRet, szMsg, sizeof(szMsg)))
    {
        return false;
    }
    return true;
}
bool QCardReader::WriteEntryInfo_NewProCard(const TCardTollInfo& CardTollInfo)
{
    TCardFileConverter sTmp;
    TPro0009Raw Pro0009;
    TPro0019Raw Pro0019Raw;
    TPro0012Raw Raw;
    quint8 PassCardInfoKey[8];
    TProCardConsumeInfo ConsumeInfo;
    quint8 LenData;
    int i;
    bool bRlt;

    //清标识站
    memset(&Pro0009, 0, sizeof(Pro0009));
    if (!WriteProBinFile(NEW_PSAM_SLOT, 0x09, false, 0, (quint8 *)&Pro0009, sizeof(Pro0009)))
    {
        return false;
    }
    // 对云通卡，作0金额的复合交易
    memset(&Raw, 0, sizeof(Raw));
    if (! sTmp.CardTollInfo2Pro0012Raw(Raw, CardTollInfo))
    {
        // 不应出现的失败
        LogMsg("lane",tr("CardTollInfo2Pro0012Raw Error!"));
        return false;
    }

    LenData=0;
    for(i=0;i<=2;i++)
    {
        bRlt= ProCapp(NEW_PSAM_SLOT, ConsumeInfo, 0, CardTollInfo.PassTime, (quint8 *)&Raw);
        if (! bRlt)
        {
            continue;
        }
        memset(&Pro0019Raw, 0, sizeof(Pro0019Raw));
        if(!ReadProFile(0x19, 0, 43,(quint8 *)&Pro0019Raw, LenData))
        {
            continue;
        }
        if (memcmp((quint8 *)&Raw,(quint8 *)(&Pro0019Raw)+3,sizeof(TPro0012Raw)) == 0)
        {
            AudioControl(2, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("非现金卡WriteEntryInfo读回比较不一致！"));
        }
    }
    return false;
}

bool QCardReader::WriteEntryInfo_OldProCard(const TCardTollInfo& CardTollInfo)
{
    TCardFileConverter sTmp;
    TPro0019Raw Pro0019Raw;
    TPro0012Raw Raw;
    quint8 PassCardInfoKey[8];
    TProCardConsumeInfo ConsumeInfo;
    quint8 LenData;
    int i;
    bool bRlt;

    // 对云通卡，作0金额的复合交易

    memset(&Raw, 0, sizeof(Raw));
    if (! sTmp.CardTollInfo2Pro0012Raw(Raw, CardTollInfo))
    {
        // 不应出现的失败
        LogMsg("lane",tr("CardTollInfo2Pro0012Raw Error!"));
        return false;
    }
    LenData=0;
    for(i=0;i<=2;i++)
    {
        bRlt= ProCappForOldCard(OLD_PSAM_SLOT, ConsumeInfo, 0,
                                CardTollInfo.PassTime, (quint8 *)&Raw);
        if (! bRlt)
        {
            continue;
        }
        memset(&Pro0019Raw, 0, sizeof(Pro0019Raw));
        if(!ReadProFile(0x19, 0, 43,(quint8 *)&Pro0019Raw, LenData))
        {
            continue;
        }
        if (memcmp((quint8 *)&Raw,(quint8 *)(&Pro0019Raw)+3,sizeof(TPro0012Raw)) == 0)
        {
            AudioControl(2, 1);
            return true;

        }
        else
        {

            LogMsg("lane",tr("旧非现金卡WriteEntryInfo读回比较不一致！"));
        }
    }
    return false;
}
bool QCardReader::WriteEntryInfo_NewPassCard(const TCardTollInfo& CardTollInfo)
{
    TCardFileConverter sTmp;
    int i;
    // 通行卡
    TTollCardTollInfoRaw TollCardTollInfoRaw, checkTollInfoRaw ;

    if (!SelPsamCardFile(NEW_PSAM_SLOT, 0xDF02))
    {
        return false;
    }

    // 通行卡
    memset(&TollCardTollInfoRaw, 0,sizeof(TTollCardTollInfoRaw));
    if (!sTmp.CardTollInfo2TollCardTollInfoRaw(TollCardTollInfoRaw,CardTollInfo))
    {
        return false;
    }
    //M1卡清标识站
    memset(TollCardTollInfoRaw.baFlagStaInfo, 0,sizeof(TollCardTollInfoRaw.baFlagStaInfo));
    //M1卡使用次数累加
    TollCardTollInfoRaw.dwWriteCount++;
    for(i=0;i<=2;i++)
    {
        if(JT_WriteFile(m_nComID,0x2001,KEYB,0,6,(quint8*)&TollCardTollInfoRaw)!=0)
        {
            continue;
        }
#ifdef Q_OS_WIN
        return true;
#else
        memset(&checkTollInfoRaw, 0,sizeof(TTollCardTollInfoRaw));
        if (JT_ReadFile(m_nComID,0x2001,KEYA,0,6,(quint8*)&checkTollInfoRaw)!=0)
        {
            continue;
        }
        if (memcmp(&TollCardTollInfoRaw,&checkTollInfoRaw,sizeof(TTollCardTollInfoRaw)) == 0)
        {
            AudioControl(1, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("新密钥通行卡WriteEntryInfo读回比较不一致！"));
        }
#endif
    }
    return false;
}
bool QCardReader::WriteExitInfo_NewPassCard(quint32 dwBoxNo)
{
    TTollCardTollInfoRaw TollCardTollInfoRaw;
    TTollCardTollInfoRaw checkTollCardTollInfoRaw;
    memset(&TollCardTollInfoRaw, 0, sizeof(TollCardTollInfoRaw));
    memset(&checkTollCardTollInfoRaw, 0, sizeof(checkTollCardTollInfoRaw));
    int nRt = 0;
    memcpy(&TollCardTollInfoRaw,
           &m_TollInfoWriteFailureProtector.m_TollCardTollInfoRaw,
           sizeof(TTollCardTollInfoRaw));
    TollCardTollInfoRaw.bMaintenanceAndbPassStatus= (TollCardTollInfoRaw.bMaintenanceAndbPassStatus) & 0xf0;
    TollCardTollInfoRaw.dwCardBoxId= dwBoxNo;
    TollCardTollInfoRaw.dwWriteCount++;

    for(int i=0;i<=2;i++)
    {
        nRt = JT_WriteFile(m_nComID,0x2001,KEYB,0,6,(quint8 *)&TollCardTollInfoRaw);

        LogMsg("lane",tr("----------------JT_WriteFile---------------"));
        char *tmp = HexStr((quint8 *)&TollCardTollInfoRaw,sizeof(TollCardTollInfoRaw));
        LogMsg("lane",tr("JT_WriteFile:[%1]").arg(tmp));
        if(nRt!= 0)
        {
            continue;
        }
        //qMemSet(&checkTollCardTollInfoRaw, 0,sizeof(TTollCardTollInfoRaw));
        int ret = JT_ReadFile(m_nComID,0x2001,KEYA,0,6,(quint8 *)&checkTollCardTollInfoRaw);
        LogMsg("lane",tr("----------------JT_ReadFile---------------"));
        char *tmp1 = HexStr((quint8 *)&checkTollCardTollInfoRaw,sizeof(checkTollCardTollInfoRaw));
        LogMsg("lane",tr("JT_WriteFile:[%1]").arg(tmp1));

        LogMsg("lane",tr("checkTollCardTollInfoRaw.bRecIndex:[%1]！").arg(checkTollCardTollInfoRaw.dwOpId));
        if (ret!=0)
        {
            continue;
        }
        AudioControl(1, 1);
        return true;

    }
    return false;
}
bool QCardReader::WriteExitInfo_OldPassCard(quint32 dwBoxNo)
{
    TCardFileConverter sTmp;
    TOldTollCardTollInfoRaw TollCardTollInfoRaw, checkTollCardTollInfoRaw;
    int i, nRt;
    if (!SelPsamCardFile(OLD_PSAM_SLOT, 0x1003))
    {
        return false;
    }
    TollCardTollInfoRaw= m_TollInfoWriteFailureProtector.m_OldTollInfoRaw;
    TollCardTollInfoRaw.File2002.CardBoxNo= swapDword(dwBoxNo);
    TollCardTollInfoRaw.File2001.Counter=swapDword(swapDword(TollCardTollInfoRaw.File2001.Counter)+1);

    TollCardTollInfoRaw.File2001.PortFlag= 1;

    QDateTime m_qCurrentDatatime=QDateTime::currentDateTime();
    TollCardTollInfoRaw.File2002.ExTime= swapDword(SystemTime2CardTime(m_qCurrentDatatime));
    TollCardTollInfoRaw.File2002.ExNetID = swapWORD((quint16)(3201));
    memset(TollCardTollInfoRaw.File2002.ExOperatorID, 0,sizeof(TollCardTollInfoRaw.File2002.ExOperatorID));

    for(i=0;i<=2;i++)
    {
        memset(&checkTollCardTollInfoRaw,0,sizeof(checkTollCardTollInfoRaw));

        nRt= JT_WriteFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&TollCardTollInfoRaw.File2001);
        if(nRt!=0)
        {
            LogMsg("lane",tr("写旧卡2001失败：[%1]").arg(nRt));
            continue;
        }
        nRt= JT_ReadFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&checkTollCardTollInfoRaw.File2001);
        if ( nRt!= 0)
        {
            LogMsg("lane",tr("写旧卡2001读回准备校验失败：[%1]").arg(nRt));
            continue;
        }

        nRt = JT_WriteFile(m_nComID,0x2002,OLD_KEYA,0,3,(quint8 *)&TollCardTollInfoRaw.File2002);
        if(nRt!= 0)
        {
            LogMsg("lane",tr("写旧卡2002失败：[%1]").arg(nRt));
            continue;
        }
        nRt= JT_ReadFile(m_nComID,0x2002,OLD_KEYA,0,3,(quint8 *)&checkTollCardTollInfoRaw.File2002);
        if (nRt!= 0)
        {
            LogMsg("lane",tr("写旧卡2002读回准备校验失败：[%1]").arg(nRt));
            continue;
        }

        if (memcmp(&checkTollCardTollInfoRaw.File2001,&TollCardTollInfoRaw.File2001,sizeof(checkTollCardTollInfoRaw.File2001)) == 0)
        {
            AudioControl(1, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("旧密钥通行卡WriteExitInfo读回比较不一致！"));
        }
    }
    return false;
}
bool QCardReader::WriteBack_NewPassCard(QString sCardId)
{
    TTollCardTollInfoRaw checkTollCardTollInfoRaw;
    int i, nRlt;
    if (sCardId != m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo())
    {
        LogMsg("lane",tr("回写卡错误，卡序列号为:[%1],保存信息卡序列号为:[%2]").arg(sCardId).arg(m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo()));
        return false;
    }
    if (!SelPsamCardFile(NEW_PSAM_SLOT, 0xDF02))
    {
        return false;
    }
    for(i=0;i<=2;i++)
    {

        nRlt= JT_WriteFile(m_nComID, 0x2001, KEYB, 0, 6,(quint8 *)&m_TollInfoWriteFailureProtector.m_TollCardTollInfoRaw);
        if (nRlt!=0)
        {
            continue;
        }
        memset(&checkTollCardTollInfoRaw, 0, sizeof(checkTollCardTollInfoRaw));
        nRlt = JT_ReadFile(m_nComID, 0x2001, KEYA, 0, 6,(quint8 *)&checkTollCardTollInfoRaw);
        if (nRlt!=0)
        {
            continue;
        }
        if (memcmp(&checkTollCardTollInfoRaw,&m_TollInfoWriteFailureProtector.m_TollCardTollInfoRaw,sizeof(checkTollCardTollInfoRaw)) == 0)
        {
            AudioControl(1, 1);
            return true;
        }

        else
        {
            LogMsg("lane",tr("新密钥通行卡回写卡读回比较不一致！"));
        }
    }
    return false;

}
bool QCardReader::WriteBack_OldPassCard(QString sCardId)
{
    TOldTollCardTollInfoRaw checkTollCardTollInfoRaw;
    int i, nRlt;
    if (sCardId!= m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo())
    {
        LogMsg("lane",tr("回写卡错误，卡序列号为:[%1],保存信息卡序列号为:[%2]").arg(sCardId).arg(m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo()));
        return false;
    }

    if (!SelPsamCardFile(OLD_PSAM_SLOT, 0x1003))
    {
        return false;
    }


    for(i=0;i<=2;i++)
    {
        memset(&checkTollCardTollInfoRaw, 0, sizeof(checkTollCardTollInfoRaw));
        nRlt = JT_WriteFile(m_nComID, 0x2001, OLD_KEYA, 0, 3,(quint8 *)&m_TollInfoWriteFailureProtector.m_OldTollInfoRaw.File2001);
        if (nRlt!= 0)
        {
            continue;
        }
        nRlt= JT_ReadFile(m_nComID, 0x2001, OLD_KEYA, 0, 3,(quint8 *)&checkTollCardTollInfoRaw.File2001);
        if (nRlt!= 0)
        {
            continue;
        }
        nRlt= JT_WriteFile(m_nComID, 0x2002, OLD_KEYA, 0, 3,(quint8 *)&m_TollInfoWriteFailureProtector.m_OldTollInfoRaw.File2002);
        if (nRlt!=0)
        {
            continue;
        }
        nRlt= JT_ReadFile(m_nComID, 0x2002, OLD_KEYA, 0, 3,(quint8 *)&checkTollCardTollInfoRaw.File2002);
        if (nRlt != 0)
        {
            continue;
        }

        if (memcmp(&checkTollCardTollInfoRaw,&m_TollInfoWriteFailureProtector.m_OldTollInfoRaw,sizeof(checkTollCardTollInfoRaw)) == 0)
        {
            AudioControl(1, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("旧密钥通行卡回写卡读回比较不一致！"));
        }

    }
    return false;

}

bool QCardReader::ReadNewProCard(TPro0015Raw &Pro0015Raw, TProCardBasicInfo &ProCardBasicInfo, quint32 &dwBalance, TCardTollInfo &CardTollInfo)
{
    TCardFileConverter sTmp;
    const int MAX_LEN=256;
    char* sSelectAdfCmd="805C000204";

    // DF01复合消费专用文件       43字节
    TPro0019Raw  Pro0019Raw;
    // DF01云通卡免费信息文件    83字节
    TPro000ARaw   Pro000ARaw;
    // DF01标识站信息文件         9 字节
    TPro0009Raw  Pro0009Raw;
    quint8 nLenRead;
    quint32  dwTmp;
    int nCmdLen, nReplyLen,nRet;
    quint8 sCmd[MAX_LEN],sReply[MAX_LEN];
    quint8 sData[9];
    char  szMsg[MAX_LEN];
    QString sCardId;

    // 读新卡免费信息文件
    if (!ReadProFile(0x0A,0,83,(quint8 *)&Pro000ARaw, nLenRead))
    {
        return false;
    }
    if (!sTmp.ProBasicRaw2ProCardBasicInfo(ProCardBasicInfo,m_dwLastCardId,Pro0015Raw,Pro000ARaw))
    {
        return false;
    }
    sCardId= QString::fromLocal8Bit(ProCardBasicInfo.szCardNo, 16);
    //上次写卡失败 直接返回通行文件 不继续读0019
    if (m_TollInfoWriteFailureProtector.IsWriteFailedCard(sCardId))
    {
        if (!sTmp.Pro0012Raw2CardTollInfo(CardTollInfo,m_TollInfoWriteFailureProtector.m_Pro0012Raw,m_TollInfoWriteFailureProtector.m_Pro0009Raw))
        {
            return false;
        }
        // NOTE: 卡已经被尝试写过，肯定已通过校验，不再重复校验
        CardTollInfo.bCRC=true;
    }
    else
    {
        // 读复合消费专用文件
        if (!ReadProFile(0x19,0,43,(quint8 *)&Pro0019Raw,nLenRead))
        {
            return false;
        }
        // 读标识站信息文件
        if (!ReadProFile(0x09,0,9,(quint8 *)&Pro0009Raw,nLenRead))
        {
            return false;
        }
        if (! sTmp.Pro0012Raw2CardTollInfo(CardTollInfo,
                                           Pro0019Raw.raw0012,
                                           Pro0009Raw))
        {
            return false;
        }

        m_TollInfoWriteFailureProtector.SaveTollInfoForWriteFailure(sCardId,
                                                                    Pro0019Raw.raw0012, Pro0009Raw);
    }
    //保存读到的卡印刷号及信息
    m_sLastCardId= sCardId;
    //读余额
    nReplyLen= 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    memset(sData, 0,sizeof(sData));

    nCmdLen = strlen(sSelectAdfCmd)/ 2;
    Hex2Raw(sCmd,sSelectAdfCmd,strlen(sSelectAdfCmd));
    if (!ProCmd(sCmd, nCmdLen, sReply, nReplyLen, nRet, szMsg, 128))
    {
        return false;
    }
    memcpy(&dwTmp,sReply + 2, 4);
    dwBalance= qFromBigEndian(dwTmp);
    AudioControl(1, 1);
    return true;

}

bool QCardReader::CompareTollCardInfoRaw(TTollCardTollInfoRaw &writeInfo, TTollCardTollInfoRaw &readInfo)
{
    if(writeInfo.bMaintenanceAndbPassStatus == readInfo.bMaintenanceAndbPassStatus &&
            writeInfo.bLaneId == readInfo.bLaneId &&
            writeInfo.bRecIndex == readInfo.bRecIndex &&
            writeInfo.bShiftID == readInfo.bShiftID &&
            writeInfo.bVehClass == readInfo.bVehClass &&
            writeInfo.bVehPlateColor == readInfo.bVehPlateColor &&
            writeInfo.bVehTollType == readInfo.bVehTollType &&
            writeInfo.dwCardBoxId == readInfo.dwCardBoxId &&
            writeInfo.dwOpId == readInfo.dwOpId &&
            writeInfo.dwPassTime == readInfo.dwPassTime &&
            writeInfo.dwWriteCount == readInfo.dwWriteCount)
    {
        return true;
    }
    return false;
}

void QCardReader::StartCardDetection()
{
    m_detectThread.Resume();
}

void QCardReader::StopCardDetection()
{
    m_detectThread.Pause();
}

bool QCardReader::Read0015(quint32 dwCardSerialId)
{
    quint8 nLenRead;
    memset(&m_Pro0015Raw, 0, sizeof(m_Pro0015Raw));
    if (m_nLastCardType !=TYPE_PRO)
    {
        return false;
    }

    if (!SelProCardFile(0x1001))
    {
        return false;
    }

    // 读卡片发行基本数据文件
    if (!ReadProFile(0x15,0,43,(quint8 *)&m_Pro0015Raw, nLenRead))
    {
        return false;
    }
    return true;
}

bool QCardReader::ReadBoxCard(TBoxCardBasicInfo &BoxCardBasicInfo, TBoxCardAppInfo &BoxCardAppInfo)
{
    QMutexLocker locker(&m_mutex);

    bool bRlt = false;

    // 判断是否是M1卡
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        LogMsg("lane",tr("非M1卡"));
        return false;
    }
    //是否是新M1卡
    if (GetIsNewMifareOneCard())
    {
        bRlt= ReadNewBoxCard(BoxCardBasicInfo, BoxCardAppInfo);
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }
    else
    {
        LogMsg("lane",tr("暂不支持老标签卡"));
        bRlt = ReadOldBoxCard(BoxCardBasicInfo, BoxCardAppInfo);
        if (bRlt)
        {
            AudioControl(1, 1);
        }
        return bRlt;
    }
}

bool QCardReader::WriteBoxCard(quint32 dwCardId, const TBoxCardAppInfo &BoxCardAppInfo)
{
    bool bRlt = false;

    // 判断是否是M1卡
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        return false;
    }
    //是否是新M1卡
    if (GetIsNewMifareOneCard())
    {
        bRlt = WriteNewBoxCard(BoxCardAppInfo);
    }
    else
    {
        //暂不支持老标签卡
        bRlt = WriteOldBoxCard(BoxCardAppInfo);
    }

    // 记录是否有写卡失败发生
    if (!bRlt)
    {
        m_TollInfoWriteFailureProtector.MarkWriteFailure(m_sLastCardId);
    }
    else
    {
        m_TollInfoWriteFailureProtector.MarkWriteSuccess(m_sLastCardId);
    }
    return bRlt;
}

bool QCardReader::ReadNewBoxCard(TBoxCardBasicInfo &BoxCardBasicInfo,
                                 TBoxCardAppInfo &BoxCardAppInfo)
{
    TCardFileConverter sTmp;
    const char *szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10];
    quint8 sReply[256];
    char szMsg[128];
    TBoxCardBasicInfoRaw BasicInfoRaw ;  //1001文件
    TBoxCardAppInfoRaw AppInfoRaw;            //2001文件
    QString sCardID;
    //M1卡密钥目录
    nSockId= NEW_PSAM_SLOT;
    nReplyLen= 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen= strlen(szSelectAdfCmd)/ 2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,sReply, nReplyLen, nRlt, szMsg, 128))
    {
        return false;
    }
    memset(&BasicInfoRaw, 0xff, sizeof(BasicInfoRaw));
    memset(&AppInfoRaw, 0xff,sizeof(AppInfoRaw));
    nRlt = JT_ReadFile(m_nComID, 0x1001, KEYA, 0, 2, (quint8 *)&BasicInfoRaw);
    if (nRlt!=0)
    {
        LogMsg("lane",tr("读卡盒卡1001文件出错:[%1]").arg(nRlt));
        return false;
    }
    if(!sTmp.BoxCardBasicInfoRaw2BoxCardBasicInfo(BoxCardBasicInfo, m_dwLastCardId, BasicInfoRaw))
    {
        return false;
    }
    if (BasicInfoRaw.M1BasicInfo.bType!= CARD_TYPE_BOX_CARD)
    {
        return false;
    }

    sCardID = QString::number(BoxCardBasicInfo.M1BasicInfo.dwCardNum);
    if (m_TollInfoWriteFailureProtector.IsWriteFailedCard(sCardID))
    {
        LogMsg("lane",tr("卡印刷号[%1]的卡片上次写卡盒信息失败，使用首次读入的卡盒信息").arg(sCardID));
        if (!sTmp.BoxCardAppInfoRaw2BoxCardAppInfo(BoxCardAppInfo,
                                                   BoxCardBasicInfo.M1BasicInfo.dwCardNum,     //使用表面印刷号做卡盒号
                                                   m_TollInfoWriteFailureProtector.m_BoxCardAppInfoRaw))
        {
            return false;
        }
        BoxCardAppInfo.bCRC = true;

    }
    else
    {
        if (JT_ReadFile(m_nComID,0x2001,KEYA,0,3,(quint8 *)&AppInfoRaw)!=0)
        {
            return false;
        }
        if(!sTmp.BoxCardAppInfoRaw2BoxCardAppInfo(BoxCardAppInfo,
                                                  BoxCardBasicInfo.M1BasicInfo.dwCardNum,
                                                  AppInfoRaw))
        {
            return false;
        }
        m_TollInfoWriteFailureProtector.SaveTollInfoForWriteFailure(sCardID, AppInfoRaw);
    }
    m_dwLastCardNo= BoxCardBasicInfo.M1BasicInfo.dwCardNum;
    m_sLastCardId = sCardID;
    return true;
}
bool QCardReader::ReadOldBoxCard(TBoxCardBasicInfo &BoxCardBasicInfo, TBoxCardAppInfo &BoxCardAppInfo)
{
    TCardFileConverter sTmp;
    TOldBoxCardBasicInfoRaw BasicInfoRaw;
    int nRlt,i;
    TOldTollCardTollInfo2001Raw AppInfoRaw;
    QString strTemp;
    quint8* pB;
    QString sCardID;
    if (!SelPsamCardFile(OLD_PSAM_SLOT, 0x1003))
    {
        return false;
    }
    nRlt= JT_ReadFile(m_nComID,0x1001,OLD_KEYA,0,3,(quint8 *)&BasicInfoRaw);
    if (nRlt!= 0)
    {
        return false;
    }

    if (!sTmp.Old_BoxCardBasicInfoRaw2BoxCardBasicInfo(BoxCardBasicInfo,m_dwLastCardId,BasicInfoRaw))
    {
        return false;
    }
    if (BoxCardBasicInfo.M1BasicInfo.bType != CARD_TYPE_BOX_CARD)
    {
        // 卡类型已转换成功，不是标签卡，不必继续处理
        BoxCardAppInfo.clear();
        return false;
    }

    sCardID =QString::number(BoxCardBasicInfo.M1BasicInfo.dwCardNum);
    if (m_TollInfoWriteFailureProtector.IsWriteFailedCard(sCardID))
    {
        LogMsg("lane",tr("卡印刷号[%1]的卡片上次写卡盒信息失败，使用首次读入的卡盒信息").arg(sCardID));
        if (!sTmp.Old_BoxCardAppInfoRaw2BoxCardAppInfo(
                    BoxCardAppInfo,
                    BoxCardBasicInfo.M1BasicInfo.dwCardNum,     //使用表面印刷号做卡盒号
                    m_TollInfoWriteFailureProtector.m_OldBoxCardAppInfoRaw))
        {
            return false;
        }
        // NOTE: 卡已经被尝试写过，肯定已通过校验，不再重复校验
        BoxCardAppInfo.bCRC = true;

    }
    else
    {
        nRlt= JT_ReadFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&AppInfoRaw);
        if (nRlt!= 0)
        {
            return false;
        }
        strTemp="";
        pB= (quint8 *)&AppInfoRaw;
        for(i=0;i<=sizeof(AppInfoRaw);i++)
        {
            strTemp = strTemp + tr("%1").arg(*pB, 2, 16, QLatin1Char('0'));
            pB++;
        }

        if (!sTmp.Old_BoxCardAppInfoRaw2BoxCardAppInfo(
                    BoxCardAppInfo,
                    BoxCardBasicInfo.M1BasicInfo.dwCardNum,     //使用表面印刷号做卡盒号
                    AppInfoRaw))
        {
            return false;
        }
        m_TollInfoWriteFailureProtector.SaveTollInfoForWriteFailure(sCardID, AppInfoRaw);
    }
    m_dwLastCardNo = BoxCardBasicInfo.M1BasicInfo.dwCardNum;
    m_sLastCardId = sCardID;
    return true;

}

bool QCardReader::WriteNewBoxCard(const TBoxCardAppInfo &BoxCardAppInfo)
{
    TCardFileConverter sTmp;
    const char* szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10],sReply[256];
    char szMsg[128];
    TBoxCardAppInfoRaw AppInfoRaw;          //2001文件
    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen = strlen(szSelectAdfCmd)/2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,
                sReply, nReplyLen, nRlt, szMsg, 128))
    {
        LogMsg("lane",tr("计算密钥错误"));
    }
    memset(&AppInfoRaw, 0, sizeof(AppInfoRaw));
    if(!sTmp.BoxCardAppInfo2BoxCardAppInfoRaw(AppInfoRaw,BoxCardAppInfo))
    {
        return false;
    }
    nRlt= JT_WriteFile(m_nComID, 0x2001, KEYB, 0, 6, (quint8 *)&AppInfoRaw);
    if (nRlt!=0 )
    {
        LogMsg("lane",tr("写卡盒卡2001文件失败:[%1]").arg(nRlt));
        return false;
    }
    else
    {
        LogMsg("lane",tr("写卡盒卡2001文件成功"));
    }

    AudioControl(2,1);
    return true;
}

bool QCardReader::ReadOpCardDll(TDLLOpCardBasicInfo &OpCardBasicInfo)
{
    QMutexLocker locker(&m_mutex);

    TCardFileConverter sTmp;
    const char *szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10],sReply[256];
    char szMsg[128];
    TOpCardBasicInfoRaw  OpCardBasicInfoRaw ;  //1001文件
    TOpCardInfoRaw  OpCardInfoRaw ;            //1002文件
    // 判断是否是M1卡
    if (!GetIsMifareOneCard(CARD_TYPE_PHYSICAL(m_nLastCardType)))
    {
        // NOTE: 程序不应走到这一步
        return false;
    }

    //是否是新M1卡
    if (!GetIsNewMifareOneCard())
    {
        //不支持老身份卡
        return false;
    }

    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen= strlen(szSelectAdfCmd)/ 2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,
                sReply, nReplyLen, nRlt, szMsg, 128))
    {
        return false;
    }

    m_OpWriteCard = false;
    memset(&OpCardBasicInfoRaw, 0, sizeof(OpCardBasicInfoRaw));
    memset(&OpCardInfoRaw, 0,sizeof(OpCardInfoRaw));
    if (JT_ReadFile(m_nComID, 0x1001, KEYA, 0, 3, (quint8 *)&OpCardBasicInfoRaw)!= 0)
    {
        return false;
    }
    if (OpCardBasicInfoRaw.M1BasicInfo.bType != CARD_TYPE_OP_CARD)
    {
        // 非身份卡，不继续读其它信息，但应该将基本信息文件中的M1BasicInfo转换完成，给pOpCardBasicInfo部分赋值！
        if(!sTmp.DLLOpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo,
                                                       m_dwLastCardId, OpCardBasicInfoRaw))
        {
            return false;
        }
        return false;
    }

    if (JT_ReadFile(m_nComID,0x2001,KEYA,0,3,(quint8 *)&OpCardInfoRaw)!=0)
    {
        return false;
    }

    if(!sTmp.DLLOpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo,
                                                   m_dwLastCardId, OpCardBasicInfoRaw, OpCardInfoRaw))
    {
        return false;
    }


    AudioControl(1, 1);
    return true;
}

bool QCardReader::WriteProCard(const TProCardBasicInfo &ProCardBasicInfo)
{
    TCardFileConverter sTmp;
    TPro0015Raw Pro0015Raw;
    TPro000ARaw Pro000ARaw;
    //转换文件
    sTmp.ProCardBasicInfo2ProBasicRaw(Pro0015Raw, Pro000ARaw,ProCardBasicInfo);
    if(!WriteProBinFile(NEW_PSAM_SLOT, 0x15, true, 20, ((quint8 *)Pro0015Raw.StartTime), 23))
    {
        return false;
    }
    return true;
}

bool QCardReader::ReadMonthCard(TMonthCardBasicInfo &CardBasicInfo)
{
    QMutexLocker locker(&m_mutex);

    bool bRlt;

    if (m_nLastCardType == TYPE_PRO)
    {
        return false;
    }
    else if (GetIsNewMifareOneCard())
    {
        return false;
    }
    else
    {
        return false;
    }
    return false;
}

bool QCardReader::WriteBack_NewProCard(QString sCardId)
{
    TProCardConsumeInfo ConsumeInfo;
    QDateTime ConsumeTime;
    bool bRlt = false;
    TPro0019Raw Pro0019Raw;
    quint8 LenData;

    if (sCardId!= m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo())
    {
        LogMsg("lane",tr("回写卡错误，卡印刷号为[%1]，保存信息卡印刷号为[%2]").arg(sCardId).arg(m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo()));
        return false;
    }

    ConsumeTime= QDateTime::currentDateTime();
    for(int i=0;i<=2;i++)
    {
        bRlt= ProCapp(NEW_PSAM_SLOT, ConsumeInfo, 0, ConsumeTime,(quint8 *)&m_TollInfoWriteFailureProtector.m_Pro0012Raw);
        if (!bRlt)
        {
            continue;
        }
        memset(&Pro0019Raw, 0, sizeof(Pro0019Raw));
        if(!ReadProFile(0x19, 0, 43,((quint8 *)&Pro0019Raw), LenData))
        {
            continue;
        }
        if (memcmp((quint8 *)&m_TollInfoWriteFailureProtector.m_Pro0012Raw,(quint8 *)(&Pro0019Raw) + 3,sizeof(TPro0012Raw)) == 0)
        {
            AudioControl(2, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("新非现金卡WriteBack读回比较不一致！"));
        }
    }

    return false;
}
bool QCardReader::WriteBack_OldProCard(QString sCardId)
{
    TProCardConsumeInfo ConsumeInfo;
    bool bRlt = false;
    TPro0019Raw Pro0019Raw;
    quint8 LenData;

    if (sCardId !=m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo())
    {
        LogMsg("lane",tr("回写卡错误，卡印刷号为[%1]，保存信息卡印刷号为[%2]").arg(sCardId).arg(m_TollInfoWriteFailureProtector.GetCardIdOfSavedInfo()));
        return false;
    }

   QDateTime ConsumeTime =QDateTime::currentDateTime();
    // for i:=0 to 2 do
    for(int i=0;i<=2;i++)
    {
        bRlt= ProCappForOldCard(OLD_PSAM_SLOT, ConsumeInfo, 0, ConsumeTime,(quint8 *)&m_TollInfoWriteFailureProtector.m_OldPro0012Raw);
        if (!bRlt)
        {
            continue;
        }
        memset(&Pro0019Raw, 0, sizeof(Pro0019Raw));
        if(!ReadProFile(0x19, 0, 43,(quint8 *)&Pro0019Raw, LenData))
        {
            continue;
        }
        if(memcmp((quint8 *)&m_TollInfoWriteFailureProtector.m_OldPro0012Raw,(quint8 *)(&Pro0019Raw)+3,sizeof(TPro0012Raw)))
        {
            AudioControl(2, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("旧非现金卡WriteBack读回比较不一致！"));
        }
    }
    return false;
}

QString TTollInfoWriteFailureProtector::GetCardIdOfSavedInfo()
{
    return m_sCardIdOfSavedInfo;
}

bool TTollInfoWriteFailureProtector::IsWriteFailedCard(const QString& sCardId)
{
    if (sCardId == "")
        return false;
    return sCardId == m_sCardIdOfWriteFailedCard;
}

void TTollInfoWriteFailureProtector::MarkWriteFailure(const QString& sCardId)
{
    if (sCardId == m_sCardIdOfSavedInfo)
        m_sCardIdOfWriteFailedCard = sCardId;

    else

        // 应该不会走到这一步
        m_sCardIdOfWriteFailedCard ="";
}

void TTollInfoWriteFailureProtector::MarkWriteSuccess(const QString& sCardId)
{
    m_sCardIdOfWriteFailedCard ="";
}

void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId, const TOldTollCardTollInfoRaw &OldTollInfoRaw)
{
    if (sCardId == m_sCardIdOfSavedInfo)
    {
        return;
    }
    memcpy(&m_OldTollInfoRaw,&OldTollInfoRaw,sizeof(OldTollInfoRaw));
    m_sCardIdOfSavedInfo= sCardId;
}
void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId,const TTollCardTollInfoRaw &TollInfoRaw)
{
    if (sCardId == m_sCardIdOfSavedInfo)
    {
        // 非首次读入，忽略
        return;
    }
    memcpy(&m_TollCardTollInfoRaw, &TollInfoRaw, sizeof(TollInfoRaw));
    m_sCardIdOfSavedInfo = sCardId;
}
void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId,const TPro0012Raw &Pro0012Raw, const TPro0009Raw& Pro0009Raw)
{
    if (sCardId==m_sCardIdOfSavedInfo)
    {
        // 非首次读入，忽略
        return;
    }

    memcpy(&m_Pro0012Raw,&Pro0012Raw,sizeof(Pro0012Raw));
     memcpy(&m_Pro0009Raw,&Pro0009Raw,sizeof(Pro0009Raw));
    m_sCardIdOfSavedInfo= sCardId;
}
void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId, const TOldPro0012Raw &OldPro0012Raw)
{
    if (sCardId==m_sCardIdOfSavedInfo)
    {
        // 非首次读入，忽略
        return;
    }
    memcpy(&m_OldPro0012Raw,&OldPro0012Raw,sizeof(OldPro0012Raw));
    m_sCardIdOfSavedInfo = sCardId;
}

void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId, const TOldTollCardTollInfo2001Raw& OldMOne2001Raw)
{
    if (sCardId==m_sCardIdOfSavedInfo)
    {
        // 非首次读入，忽略
        return;
    }
    memcpy(&m_OldBoxCardAppInfoRaw,&OldMOne2001Raw,sizeof(OldMOne2001Raw));

    m_sCardIdOfSavedInfo= sCardId;
}
void TTollInfoWriteFailureProtector::SaveTollInfoForWriteFailure(const QString& sCardId, const TBoxCardAppInfoRaw &BoxCardAppRaw)
{
    if (sCardId==m_sCardIdOfSavedInfo)
    {
        return;
    }
     memcpy(&m_BoxCardAppInfoRaw,&BoxCardAppRaw,sizeof(BoxCardAppRaw));
    m_sCardIdOfSavedInfo= sCardId;
}

bool QCardReader::ReadOldOpCard(TOpCardBasicInfo &OpCardBasicInfo, TOpCardAppInfo &OpCardAppInfo)
{
    return false;
}

bool QCardReader::WriteOldBoxCard(const TBoxCardAppInfo &BoxCardAppInfo)
{
    //标签卡
    TOldTollCardTollInfo2001Raw AppInfoRaw, checkInfoRaw;
    int nRlt;
    // 标签卡
    memset(&AppInfoRaw, 0,sizeof(AppInfoRaw));
    //读取卡2001文件
    if (!SelPsamCardFile(OLD_PSAM_SLOT, 0x1003))
    {
        return false;
    }
    nRlt = JT_ReadFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&AppInfoRaw);
    if (nRlt!=0)
    {
        return false;
    }
    //读取后只改变卡数
    AppInfoRaw.Counter= swapDword(BoxCardAppInfo.dwCount);

    for(int i=0;i<=2;i++)
    {
        memset(&checkInfoRaw, 0,sizeof(checkInfoRaw));
        if(JT_WriteFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&AppInfoRaw)!= 0)
        {
            continue;
        }
        if (JT_ReadFile(m_nComID,0x2001,OLD_KEYA,0,3,(quint8 *)&checkInfoRaw)!= 0)
        {
            continue;
        }
        if (memcmp(&AppInfoRaw,&checkInfoRaw, sizeof(AppInfoRaw)) == 0)
        {
            AudioControl(1, 1);
            return true;
        }
        else
        {
            LogMsg("lane",tr("旧密钥标签卡写卡与读回比较不一致！"));
        }

    }
    return false;
}
bool QCardReader::ReadNewOpCard(TOpCardBasicInfo &OpCardBasicInfo,TOpCardAppInfo &OpCardAppInfo)
{
    TCardFileConverter sTmp;
    const char *szSelectAdfCmd="00A4000002DF02";
    int nSockId,  nCmdLen, nReplyLen, nRlt;
    quint8 sCmd[10];
    quint8 sReply[256];
    char szMsg[128];
    TOpCardBasicInfoRaw OpCardBasicInfoRaw ;  //1001文件
    TOpCardInfoRaw      OpCardInfoRaw ;            //1002文件
    TOpCardAppInfoRaw OpCardAppInfoRaw ;
    //M1卡密钥目录
    nSockId = NEW_PSAM_SLOT;
    nReplyLen = 16;
    memset(sCmd, 0,sizeof(sCmd));
    memset(sReply, 0,sizeof(sReply));
    nCmdLen = strlen(szSelectAdfCmd)/2;
    Hex2Raw(sCmd, szSelectAdfCmd, strlen(szSelectAdfCmd));
    if (!SamCmd(nSockId, sCmd, nCmdLen,sReply, nReplyLen, nRlt, szMsg, 128))
    {
        return false;
    }

    m_OpWriteCard= false;
    memset(&OpCardBasicInfoRaw, 0, sizeof(OpCardBasicInfoRaw));
    memset(&OpCardInfoRaw, 0,sizeof(OpCardInfoRaw));
    if (JT_ReadFile(m_nComID, 0x1001, KEYA, 0, 3, (quint8 *)&OpCardBasicInfoRaw)!= 0)
    {
        return false;
    }

    if (OpCardBasicInfoRaw.M1BasicInfo.bType != CARD_TYPE_OP_CARD)
    {
        // 非身份卡，不继续读其它信息，但应该将基本信息文件中的M1BasicInfo转换完成，给pOpCardBasicInfo部分赋值！
        if(!sTmp.OpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo, m_dwLastCardId, OpCardBasicInfoRaw))
        {
            return false;
        }
        return false;
    }

    if (JT_ReadFile(m_nComID,0x2001,KEYA,0,3,(quint8 *)&OpCardInfoRaw) !=0)
    {
        return false;
    }

    if(!sTmp.OpCardBasicInfoRaw2OpCardBasicInfo(OpCardBasicInfo,
                                                m_dwLastCardId, OpCardBasicInfoRaw, OpCardInfoRaw))
    {
        return false;
    }

    memset(&OpCardAppInfoRaw, 0, sizeof(OpCardAppInfoRaw));
    if (JT_ReadFile(m_nComID, 0x3001, KEYA, 0, 3, (quint8 *)&OpCardAppInfoRaw)!= 0)
    {
        return false;
    }
    if (!sTmp.OpCardAppInfoRaw2OpCardAppInfo(OpCardAppInfo,
                                             OpCardAppInfoRaw))
    {
        return false;
    }

    AudioControl(1, 1);

    return true;
}

void CardDetectThread::SetCardReader(QCardReader* pReader)
{
    m_pReader = pReader;
}

void CardDetectThread::RunOnce()
{
    if(m_pReader != NULL)
    {
        if(m_pReader->isOpen())
        {
            m_pReader->CloseCard();
            if(m_pReader->OpenCard())
            {
                emit hasOneCard(m_pReader->GetCardSerialID(), m_pReader->GetCardType());
            }
            else
            {
                emit hasNoCard();
            }
        }
    }
    msleep(500);
}

CardDetectThread::CardDetectThread()
{
    m_pReader = NULL;
    setObjectName("CardReaderThread");
}

CardDetectThread::~CardDetectThread()
{
}

QString GetProCardTypeName(int nCardType)
{
    switch(nCardType)
    {
    case CARD_TYPE_ALLFREE_CARD:
        return QObject::tr("全省免交");
    case CARD_TYPE_ALL_BAOJIAO_CARD:
        return QObject::tr("全省包交");
    case CARD_TYPE_ROAD_FREE_STORE:
        return QObject::tr("路段免交储值");
    case CARD_TYPE_ROAD_BAOJIAO_STORE:
        return QObject::tr("路段包交储值");
    case CARD_TYPE_ROAD_FREE_TALLY:
        return QObject::tr("路段免交记账");
    case CARD_TYPE_ROAD_BAOJIAO_TALLY:
        return QObject::tr("路段包交记账");
    default:
        return QObject::tr("未知");
    }
}
