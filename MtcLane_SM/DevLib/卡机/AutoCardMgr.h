#pragma once
#include "..\Common\ComPort.h"
#include "..\Common\AbstractDev.h"

const int EventID_BootUp=1;		//�����ϵ�
const int EventID_Failure=2;	//��������
const int EventID_CardUp=3;		//��Ƭȡ��
const int EventID_BoxLoad=4;	//����	
const int EventID_BoxUnLoad=5;	//����	
const int EventID_CardInsert=6;	//����	
const int EventID_StateChange=7;	//״̬�ı�
const int EventID_UpCardSuccess=8;	//�����ɹ�
const int EventID_UpCardFailed=9;	//����ʧ��

class CAuCardMgrListener
{
public:
	virtual void NotifyCardMgrEvent(int nMsgId,int nBoxCode)=0;
	virtual void NotifyPressButton(int nIcPos,int nBoxCode)=0;
};

struct CChannelInfo
{
public:
	char Status;	//'0'���� '1'���� '2'����
	BOOL HasCardBox;//�п���
	BOOL HasCardOn;	//�����п�
	int nCardBoxId;	//�����
	int nCardCount;	//����
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
	// ��ʱ���ص�����
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
	int m_nType;	//0,U�� 1-������
	DWORD m_dwTimerId;
	static CAutoCardMgr *m_pCardMgrForTimer;
};