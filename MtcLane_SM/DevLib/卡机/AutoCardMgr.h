#pragma once
#include "..\Common\ComPort.h"
#include "..\Common\AbstractDev.h"

const int EventID_BootUp=1;		//卡机上电
const int EventID_Failure=2;	//卡机故障
const int EventID_CardUp=3;		//卡片取走
const int EventID_BoxLoad=4;	//上箱	
const int EventID_BoxUnLoad=5;	//下箱	
const int EventID_CardInsert=6;	//备卡	
const int EventID_StateChange=7;	//状态改变
const int EventID_UpCardSuccess=8;	//发卡成功
const int EventID_UpCardFailed=9;	//发卡失败

class CAuCardMgrListener
{
public:
	virtual void NotifyCardMgrEvent(int nMsgId,int nBoxCode)=0;
	virtual void NotifyPressButton(int nIcPos,int nBoxCode)=0;
};

struct CChannelInfo
{
public:
	char Status;	//'0'正常 '1'故障 '2'坏卡
	BOOL HasCardBox;//有卡箱
	BOOL HasCardOn;	//天线有卡
	int nCardBoxId;	//卡箱号
	int nCardCount;	//卡数
public:
	void Clear()
	{
		Status='0';
		HasCardOn =FALSE;
		HasCardBox =FALSE;
		nCardCount =0;
		nCardBoxId=0;
	}
	CChannelInfo()
	{
		Clear();
	}
};



class CAutoCardMgr;
class CCardMgrComm:public CComPort
{
public:
	CCardMgrComm();
	BOOL InitCom(int nComId);
	void SetCarMgr(CAutoCardMgr *pCardMgr){m_pCardMgr= pCardMgr;}
	~CCardMgrComm();
public:
	BOOL DoTest(int nMaxCount,const SYSTEMTIME &curTime,char RSCTL=0 );
	BOOL UpCard(int nMgrIndex=0);
	BOOL BadCard(int nMgrIndex=0);
	BOOL SetCardCount(int nIndex,int nCardCount);
	BOOL SendRep(char RSCTL,BOOL bResult);
	char GetCTL(){return m_CTL;}
protected:
	virtual void OnDataReceived(char *lpBuff,DWORD dwBuffLen);
	char GetRSCTL();
private:
	BOOL SendCmd(const char *szCmd,int nLen);
private:
	CAutoCardMgr *m_pCardMgr;
	string m_sMsgRecv;
	char m_RSCTL;
	char m_CTL;
};

class CAutoCardMgr: public CAbstractDev
{
public:
	CAutoCardMgr();
	~CAutoCardMgr();
	virtual BOOL StartDev();
	virtual void CloseDev();
	BOOL InitCardMgr(int nCardMgrType);
	void SetListener(CAuCardMgrListener *pListerner){m_pListener = pListerner;}
	BOOL DoCommStr(const string &sCmd);
	BOOL GetBoxInfo(int nBoxCode,int &nBoxId,int &nCount);
	BOOL GetChannelInfo(int nBoxCode,int &nBoxId,int &nCount,BOOL &bHasCardOn,char &curStatus);
	BOOL UpCard(int nMgrIndex=0);
	BOOL BadCard(int nMgrIndex=0);
	BOOL SetCardCount(int nMgrIndex,int nCount);
	void ClearAllBadCard();
protected:
	virtual BOOL LoadDriver(){return TRUE;}
	virtual void ReleaseDriver(){}
	void DoCardMgrInfo(const string &sCmd,int nType=0);
	void DoCardBoxInfo(const string &sCmd,int nType=0);
	void OnResponseChar(char cCTL);
	void SetCardBoxInfo(int nIndex,BOOL bHasBox);
	void StartTimer();
	void DoOutTimer();
	// 定时器回调函数
	static void CALLBACK OnTimer(HWND hwnd,	UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
private:
	int m_nComID;
	int m_nCurIdUp;
	int m_nCurIdLow;

	CAuCardMgrListener* m_pListener;
	CChannelInfo m_ArChannelInfo[4];
	CCardMgrComm m_MgrComm;
	char m_CTL;
	char m_RSCTL;
	int m_nType;	//0,U创 1-电子所
	DWORD m_dwTimerId;
	static CAutoCardMgr *m_pCardMgrForTimer;
};