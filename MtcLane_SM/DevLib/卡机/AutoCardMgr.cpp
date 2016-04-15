#include "stdafx.h"
#include "AutoCardMgr.h"

const char STX='<';
const char ETX='>';

CAutoCardMgr *CAutoCardMgr::m_pCardMgrForTimer=NULL;
CAutoCardMgr::CAutoCardMgr()
{
	m_RSCTL='0';
	m_CTL='0';
	m_nType =0;
	m_dwTimerId =0;
	ChangeStatus(DEV_STATUS_CommErr);
	m_MgrComm.SetCarMgr(this);
}

CAutoCardMgr::~CAutoCardMgr()
{
	m_MgrComm.SetCarMgr(NULL);
}

BOOL CAutoCardMgr::StartDev()
{
	return TRUE;
}

BOOL CAutoCardMgr::InitCardMgr(int nCardMgrType)
{
	m_nType = nCardMgrType;
	m_nComID=ExtractComId(m_szConnStr1);
	if (0>=m_nComID)
	{
		LogFmtMsg("卡机串口号[%s]设置错误", m_szConnStr1);
		ChangeStatus(DEV_STATUS_ParamErr);
		return FALSE;
	}

	if(!m_MgrComm.InitCom(m_nComID))
	{
		LogFmtMsg(_T("卡机初始化失败,ComID:%d"),m_nComID);
		ChangeStatus(DEV_STATUS_CommErr);
		return FALSE;
	}

	SYSTEMTIME curTime;
	GetLocalTime(&curTime);
	m_MgrComm.DoTest(500,curTime);

	//ChangeStatus(DEV_STATUS_OK);
	return TRUE;
}

void CAutoCardMgr::CloseDev()
{
	m_MgrComm.Close();
}


BOOL CAutoCardMgr::DoCommStr( const string &sCmd )
{
	LogFmtMsg("接收到卡机数据:%s",sCmd.c_str());
	ChangeStatus(DEV_STATUS_OK);

	if(sCmd.length()<4)
		return FALSE;

	m_RSCTL = sCmd.at(1);
	m_CTL = sCmd.at(2);

	try
	{
		switch(m_CTL)
		{
		case 0x30:
		case 0x31:
			{
				OnResponseChar(m_CTL);
				break;//失败
			}
		case 0x41://'A'
			{
				//卡机上电回应初始化指令
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);
				if(m_pListener)
					m_pListener->NotifyCardMgrEvent(EventID_BootUp,0);
				m_MgrComm.DoTest(500,sysTime,m_RSCTL);
				break;
			}
		case 0x42://'B'
			{
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				DoCardMgrInfo(sCmd);
				break;
			}
		case 0x43://'C'
			{
				//出卡
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				if(sCmd.length()<6)
					return FALSE;
				char opRet=sCmd.at(3);//'1'上工位‘2’下工位'3'失败
				int nMid;
				if('3'==opRet)
				{
					//出卡失败
					nMid=EventID_UpCardFailed;
				}else
					nMid =EventID_UpCardSuccess;

				int nBoxCode = sCmd.at(4)-'0';	//'1''2''3''4'对应卡机编号
				//发卡成功事件
				if(m_pListener)
					m_pListener->NotifyCardMgrEvent(nMid,nBoxCode);
				break;
			}
		case 0x44:
			{
				//D 按钮事件
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				if(sCmd.length()<6)
					return FALSE;
				int nIcPos,nBoxCode;
				nIcPos = sCmd.at(3)-'0';	//'1'上‘2’下
				nBoxCode = sCmd.at(4)-'0';	//'1''2''3''4'天线位置
				if(m_pListener)
					m_pListener->NotifyPressButton(nIcPos,nBoxCode);
				break;
			}
		case 0x45:
			{
				//E卡片被取走
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				if(sCmd.length()<6)
					return FALSE;
				int nIcPos ,nIndex;
				nIcPos = sCmd.at(3)-'0';
				nIndex = sCmd.at(4)-'0';
				if(m_pListener)
					m_pListener->NotifyCardMgrEvent(EventID_CardUp,nIndex);
				break;
			}
		case 0x46://F
			{
				//卡夹编号，和卡数
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				int nType = 0;
				DoCardBoxInfo(sCmd,nType);
				break;
			}
		case 0x58:
			{
				//卡夹编号,无卡数（广州电子所）
				m_MgrComm.SendRep(m_RSCTL,TRUE);
				int nType =1;
				DoCardBoxInfo(sCmd,nType);
				break;
			}
		}
		return TRUE;
	}
	catch (...)
	{
		LogFmtMsg("解析卡机报文数据");
	}
	return FALSE;
}

void CAutoCardMgr::DoCardMgrInfo( const string &sCmd,int nType/*=0*/)
{
	if(sCmd.length()<30)
		return ;

	m_nCurIdUp=sCmd.at(3)-'0';
	m_nCurIdLow = sCmd.at(4)-'0';

	string subStr= sCmd.substr(5,sCmd.length()-5);
	for(int i=0;i<4;++i)
	{
		//'0' 正常 '1'故障 '2'有坏卡
		char tmpStatus=subStr.at(i*6);
		BOOL bHasBox =	'0'==subStr.at(i*6+1);
		BOOL bHasCardOn = '0'!=subStr.at(i*6+5);
		string sCardCount= subStr.substr(i*6+2,3);
		int nCardCount= atoi(sCardCount.c_str());

		if(bHasCardOn)
			nCardCount--;

		if(bHasCardOn != m_ArChannelInfo[i].HasCardOn)
		{
			if(bHasCardOn)
			{
				//天线有卡插入
				if(m_pListener)
				{
					m_pListener->NotifyCardMgrEvent(EventID_CardInsert,i+1);
				}
			}
		}

		if(1==nType)
		{
			if(bHasBox)
			{
				m_ArChannelInfo[i].nCardCount = nCardCount;
			}
			SetCardBoxInfo(i,bHasBox);
		}

		m_ArChannelInfo[i].HasCardBox = bHasBox;
		m_ArChannelInfo[i].HasCardOn = bHasCardOn;
		m_ArChannelInfo[i].nCardCount =nCardCount;
		m_ArChannelInfo[i].Status = tmpStatus;

		if(m_pListener)
			m_pListener->NotifyCardMgrEvent(EventID_StateChange,i+1);
	}
	return;
}

void CAutoCardMgr::DoCardBoxInfo( const string &sCmd,int nType)
{
	string sBoxInfo;
	int nBoxIdLen;
	if(0==nType)
	{
		if(sCmd.length()<40)
			return;
		sBoxInfo = sCmd.substr(3,36);
		nBoxIdLen =6;
	}else
	{
		if(sCmd.length()<36)
			return;
		sBoxInfo = sCmd.substr(3,32);
		nBoxIdLen=5;
	}

	for(int i=0;i<4;++i)
	{
		int nCardBoxId,nCardCount;
		string sBoxId= sBoxInfo.substr(i*(nBoxIdLen+3),nBoxIdLen);
		nCardBoxId=atoi(sBoxId.c_str());
		string sCount = sBoxInfo.substr(i*(nBoxIdLen+3)+nBoxIdLen,3);
		nCardCount = atoi(sCount.c_str());

		if(1)
		{
			BOOL bHasBox = nCardBoxId>0;
			if(bHasBox)
			{
				m_ArChannelInfo[i].nCardBoxId=nCardBoxId;
				m_ArChannelInfo[i].nCardCount = nCardCount;
			}
			if(0==nType)
				SetCardBoxInfo(i,nCardBoxId>0);
			if(!bHasBox)
			{
				m_ArChannelInfo[i].nCardBoxId=0;
				m_ArChannelInfo[i].nCardCount=0;
			}
		}
	}
}

void CAutoCardMgr::OnResponseChar( char cCTL )
{
	char curCtl= m_MgrComm.GetCTL();
	switch(cCTL)
	{
	case 0x30:
		{
			if('a'==curCtl)
			{
				ChangeStatus(DEV_STATUS_OK);
			}
			break;
		}
	case 0x31:
		{
			if('a'==curCtl)
			{
				ChangeStatus(DEV_STATUS_CommErr);
			}
			break;
		}
	}
}

void CAutoCardMgr::SetCardBoxInfo( int nIndex,BOOL bHasBox )
{
	if(m_ArChannelInfo[nIndex].HasCardBox != bHasBox)
	{	
		if(bHasBox)
		{
			if(m_pListener)
				m_pListener->NotifyCardMgrEvent(EventID_BoxLoad,nIndex+1);

		}else
		{
			if(m_pListener)
				m_pListener->NotifyCardMgrEvent(EventID_BoxUnLoad,nIndex+1);
		}

		m_ArChannelInfo[nIndex].HasCardBox = bHasBox;
	}
}

BOOL CAutoCardMgr::GetBoxInfo( int nBoxCode,int &nBoxId,int &nCount )
{
	if(nBoxCode<1 || nBoxCode>4)
	{
		return FALSE;
	}

	nBoxId = m_ArChannelInfo[nBoxCode-1].nCardBoxId;
	nCount = m_ArChannelInfo[nBoxCode-1].nCardCount;
	return TRUE;
}

BOOL CAutoCardMgr::GetChannelInfo( int nBoxCode,int &nBoxId,int &nCount,BOOL &bHasCardOn,char &curStatus )
{
	if(nBoxCode<1 || nBoxCode>4)
		return FALSE;

	nBoxId = m_ArChannelInfo[nBoxCode-1].nCardBoxId;
	nCount = m_ArChannelInfo[nBoxCode-1].nCardCount;
	bHasCardOn = m_ArChannelInfo[nBoxCode-1].HasCardOn;
	curStatus = m_ArChannelInfo[nBoxCode-1].Status;
	return TRUE;
}

BOOL CAutoCardMgr::UpCard( int nMgrIndex/*=0*/ )
{
	return m_MgrComm.UpCard(nMgrIndex);
}

BOOL CAutoCardMgr::BadCard( int nMgrIndex/*=0*/ )
{
	return m_MgrComm.BadCard(nMgrIndex);
}

BOOL CAutoCardMgr::SetCardCount(int nMgrIndex,int nCount)
{
	return m_MgrComm.SetCardCount(nMgrIndex,nCount);
}


void CAutoCardMgr::StartTimer()
{
	m_dwTimerId =SetTimer(NULL,1,2000,OnTimer);
	m_pCardMgrForTimer= this;
}

void CALLBACK CAutoCardMgr::OnTimer( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	KillTimer(NULL,idEvent);
	if(m_pCardMgrForTimer)
	{
		m_pCardMgrForTimer->DoOutTimer();
	}
}

void CAutoCardMgr::DoOutTimer()
{

}

void CAutoCardMgr::ClearAllBadCard()
{
	for(int i=0;i<4;++i)
	{
		if('2'==m_ArChannelInfo[i].Status)
		{
			UpCard(i+1);
		}
	}
}
CCardMgrComm::CCardMgrComm()
{
	m_pCardMgr= NULL;
	m_RSCTL ='0';
	m_CTL='0';
	m_sMsgRecv="";
	
}

CCardMgrComm::~CCardMgrComm()
{

}

void CCardMgrComm::OnDataReceived( char *lpBuff,DWORD dwBuffLen )
{
	if(!lpBuff || 0==dwBuffLen)
		return;

	string newStr="";
	newStr.assign(lpBuff,dwBuffLen);
	m_sMsgRecv+=newStr;

	while(true)
	{
		size_t nBegin=m_sMsgRecv.find(STX);
		if(string::npos==nBegin)
			return;
		
		if(nBegin>0)
			m_sMsgRecv.erase(0,nBegin);

		size_t nEnd = m_sMsgRecv.find(ETX);
		
		if(string::npos==nEnd)
			return;
		string sCmd = m_sMsgRecv.substr(0,nEnd+1);

		if(m_pCardMgr)
			m_pCardMgr->DoCommStr(sCmd);
		m_sMsgRecv.erase(0,nEnd+1);
	}
}

BOOL CCardMgrComm::InitCom( int nComId )
{
	Close();
	CString strCom;
	strCom.Format("\\\\.\\COM%d",nComId);

	long ErrorCode =Open((LPCTSTR)strCom);

	if(ERROR_SUCCESS != ErrorCode)
		return FALSE;

	ErrorCode = Setup((CSerial::EBaudrate)CSerial::EBaud9600);

	if (ERROR_SUCCESS == ErrorCode)
	{
		m_sMsgRecv ="";
		StartMonThread();
		return TRUE;
	}else
	{
		return FALSE;
	}	
}

char CCardMgrComm::GetRSCTL()
{
	char ret = m_RSCTL;
	if('9'>m_RSCTL && m_RSCTL>='0')
		m_RSCTL++;
	else
		m_RSCTL='0';
	return ret;
}

BOOL CCardMgrComm::SendCmd( const char *szCmd,int nLen )
{
	if(!szCmd || 0==nLen)
		return FALSE;
	DWORD dwWriteLen=0;

	LONG lRet = Write(szCmd,nLen,&dwWriteLen);

	if(lRet != ERROR_SUCCESS || dwWriteLen != nLen)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CCardMgrComm::DoTest( int nMaxCount,const SYSTEMTIME &curTime,char RSCTL/*=0 */ )
{
	char cRSCTL;
	if(0==RSCTL)
	{
		cRSCTL = GetRSCTL();
	}else
		cRSCTL = RSCTL;

	m_CTL = 'a';
	
	char szCmd[64]={'\0'};
	char szTime[15]={'\0'};

	FormatTimeToChar14(curTime,szTime);
	_snprintf_s(szCmd,_countof(szCmd),_TRUNCATE,"<%c%c%.3d%s>",cRSCTL,m_CTL,nMaxCount,szTime);

	return SendCmd(szCmd,strlen(szCmd));
}

BOOL CCardMgrComm::UpCard( int nMgrIndex/*=0*/ )
{
	m_CTL ='b';
	char cRsctl= GetRSCTL();
	char szCmd[64]={'\0'};
	char cMsgIndex = nMgrIndex+'0';

	_snprintf_s(szCmd,_countof(szCmd),_TRUNCATE,"<%c%c%c>",cRsctl,m_CTL,cMsgIndex);

	return SendCmd(szCmd,strlen(szCmd));
}

BOOL CCardMgrComm::BadCard( int nMgrIndex/*=0*/ )
{
	m_CTL='c';
	char cRsctl=GetRSCTL();
	char szCmd[64]={'\0'};
	char cMsgIndex = nMgrIndex+'0';
	_snprintf_s(szCmd,_countof(szCmd),_TRUNCATE,"<%c%c%c>",cRsctl,m_CTL,cMsgIndex);
	return SendCmd(szCmd,strlen(szCmd));
}

BOOL CCardMgrComm::SetCardCount( int nIndex,int nCardCount )
{
	m_CTL='g';
	char cRsctl=GetRSCTL();
	char szCmd[64]={'\0'};
	
	_snprintf_s(szCmd,_countof(szCmd),_TRUNCATE,"<%c%c%.1d%.3d>",cRsctl,m_CTL,nIndex%10,nCardCount%1000);
	return SendCmd(szCmd,strlen(szCmd));
}

BOOL CCardMgrComm::SendRep( char RSCTL,BOOL bResult )
{
	char cRet;
	if(bResult)
		cRet='0';
	else
		cRet='1';
	char szCmd[8]={'\0'};

	_snprintf_s(szCmd,_countof(szCmd),_TRUNCATE,"<%c%c>",RSCTL,cRet);
	return SendCmd(szCmd,strlen(szCmd));
}

