#ifndef QCARDREADER_H
#define QCARDREADER_H
#include <QString>
#include <QObject>
#include "BaseInterface/devbaseinterface.h"
#include "CommonModule/CardFileDef.h"
#include "CommonModule/qpausablethread.h"
#include "CommonModule/cardfileconverter.h"
const int MAX_PSAM_SOCKET = 4;
const int MAX_READER_COUNT = 4;
const int SECTOR_LEN = 48;
const int BLOCK_LEN = 16;
//旧PSAM槽位
const int OLD_PSAM_SLOT = 1;
//新PSAM槽位
const int NEW_PSAM_SLOT = 2;
const int KEYA=0;
const int KEYB=1;
const quint8 PUB_KEY[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
const quint8 OLD_KEYA = 0x10;
//动态库接口
#ifdef Q_OS_WIN
typedef int ( *Func_JT_OpenReader)(int iComID,
                                   const char *sParas);
#else
typedef int ( *Func_JT_OpenReader)(const char *dev,int iComID,
                                   int param);
#endif
typedef int ( *Func_JT_CloseReader)(int iComID);
typedef int ( *Func_JT_OpenCard)(int iComID,
                                 quint8 *sCardSer,
                                 int &iMaxLength);
typedef int ( *Func_JT_CloseCard)(int iComID);
typedef int ( *Func_JT_LEDDisplay)(int iComID,
                                   quint8 cRed,
                                   quint8 cGreen,
                                   quint8 cBlue);
typedef int ( *Func_JT_AudioControl)(int iComID,
                                     quint8 cTimes,
                                     quint8 cVoice);
typedef int ( *Func_JT_ReadBlock)(int iComID,
                                  int iKeyType,
                                  int iBlockn,
                                  const quint8 *sKey,
                                  quint8 *sReply);
typedef int ( *Func_JT_WriteBlock)(int iComID,
                                   int iKeyType,
                                   int iBlockn,
                                   const quint8*sKey,
                                   const quint8 *sData);
typedef int ( *Func_JT_WriteKey)(int iComID,
                                 int iKeyType,
                                 int iSec,
                                 const quint8 *sKey,
                                 const quint8 *sKeyData);
typedef int ( *Func_JT_CPUCCommand)(int iComID,
                                    const quint8 *sCommand,
                                    int iLenCmd,
                                    quint8 *sReply,
                                    int &iLenRep);
typedef int ( *Func_JT_SamReset)(int iComID,
                                 int iSockID,
                                 int iBaudrate,
                                 int iProtocolType,
                                 quint8 *sReply,
                                 int &iLenRep);
typedef int ( *Func_JT_ChangeSamBaudrate)(int iComID,
                                          int iSockID,
                                          int iBaudrate,
                                          int iProtocolType);
typedef int ( *Func_JT_SamCommand)(int iComID,
                                   int iSockID,
                                   const quint8 *sCommand,
                                   int iLenCmd,
                                   quint8 *sReply,
                                   int &iLenRep);
typedef int ( *Func_JT_ReadFile)(int iComID,
                                 int sFileId,
                                 quint8 sKeyType,
                                 int iAddr,
                                 int iLength,
                                 quint8 *sReply);
typedef int ( *Func_JT_WriteFile)(int iComID,
                                  int sFileID,
                                  quint8 sKeyType,
                                  int iAddr,
                                  int iLength,
                                  const quint8 *sData);
typedef int ( *Func_JT_GetLastError)(int iComID,
                                     char *sLastError,
                                     int iMaxLength);
typedef void ( *Func_JT_ReaderVersion)(int iComID,
                                       char *sReaderVersion,
                                       int iRVerMaxLength,
                                       char *sAPIVersion,
                                       int iAPIVerMaxLength);
enum  CARD_TYPE_PHYSICAL
{
    TYPE_NULL = 0,        //未知
    TYPE_M1_S50_BLOCK=1,	// S50卡块格式
    TYPE_M1_S50_MAD,		// S50卡MAD格式
    TYPE_PRO,				// PRO卡
    TYPE_M1_S70_BLOCK,		// S50卡块格式
    TYPE_M1_S70_MAD,		// S50卡MAD格式
    TYPE_PRO_OLD        //旧PRO卡
};

class QCardReader;

class CardDetectThread: public QPausableThread
{
    Q_OBJECT
signals:
    void hasOneCard(quint32 nCardId, int nCardType);
    void hasNoCard();
public:
    CardDetectThread();

    virtual ~CardDetectThread();
    //设置卡读写器指针
    void SetCardReader(QCardReader* pReader);
    QCardReader* getCardReader()
    {
        return m_pReader;
    }

protected:
    void RunOnce();
private:
    QCardReader* m_pReader;
};

class TTollInfoWriteFailureProtector
{
public:
    // 旧密钥通行卡可变信息区
    TOldTollCardTollInfoRaw m_OldTollInfoRaw;
    // 新密钥通行卡通行信息文件
    TTollCardTollInfoRaw m_TollCardTollInfoRaw;
    // 旧标签卡2001文件
    TOldTollCardTollInfo2001Raw m_OldBoxCardAppInfoRaw;
    // 新标签卡2001文件
    TBoxCardAppInfoRaw m_BoxCardAppInfoRaw;
    // 云通卡通行信息文件
    TPro0012Raw m_Pro0012Raw;
    // 云通卡标识站信息文件
    TPro0009Raw m_Pro0009Raw;
    // 旧非现金卡
    TOldPro0012Raw m_OldPro0012Raw;
protected:
    // 本类中保存的信息对应的卡印刷号
    QString m_sCardIdOfSavedInfo;
    // 上次写卡失败的卡印刷号
    QString m_sCardIdOfWriteFailedCard;
public:
    //本类中保存的信息对应的卡印刷号
    QString GetCardIdOfSavedInfo();
    // 标记写卡成功
    void MarkWriteSuccess(const QString& sCardId);
    // 标记写卡失败
    void MarkWriteFailure(const QString& sCardId);
    // 判断该卡是否为写失败的卡
    bool IsWriteFailedCard(const QString& sCardId);
    // 保存首次读入的旧密钥通行卡通行信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId, const TOldTollCardTollInfoRaw& OldTollInfoRaw);
    // 保存首次读入的新密钥通行卡通行信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId, const TTollCardTollInfoRaw &TollInfoRaw);
    // 保存首次读入的云通卡通行信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId,const TPro0012Raw &Pro0012Raw,const TPro0009Raw &Pro0009Raw);
    // 保存首次读入的原支付卡通行信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId,const TOldPro0012Raw &OldPro0012Raw);
    // 保存首次读入的原卡盒卡信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId,const TOldTollCardTollInfo2001Raw &OldMOne2001Raw) ;
    // 保存首次读入的卡盒卡信息
    virtual void SaveTollInfoForWriteFailure(const QString& sCardId, const TBoxCardAppInfoRaw &BoxCardAppRaw) ;
};

class QCardReader : public DevBaseInterface
{
    Q_OBJECT

public:
    explicit QCardReader(const QString& sName,QObject *parent = 0);

signals:

public slots:
protected:
    virtual bool LoadFunction();
    virtual bool ReleaseFunction();
    virtual bool Init();
    virtual bool Close();

public:
    //打开卡片，并保存卡片的序列号、卡片类型
    virtual bool OpenCard();
public:
    //打开卡片
    virtual int OpenCard(unsigned char *pCardSer,int &nMaxLength);
    //关闭卡片
    int CloseCard();

    int LEDDisplay(quint8 cRed,quint8 cGreen,quint8 cBlue );
    int AudioControl(quint8 cTimes,quint8 cVoice );
    int ReadBlock(int iKeyType,int iBlockn,
                  const quint8 *sKey,quint8 *sReply);
    int WriteBlock(int iKeyType,int iBlockn,
                   const quint8 *sKey, const quint8 *sData);
    int CPUCCommand(const quint8 *sCommand,int iLenCmd,
                    quint8 *sReply, int &iLenRep);
    int SamReset(int iSockID,int iBaudrate,int iProtocolType,
                 quint8 * sReply, int &iLenRep);
    int ChangeSamBaudrate(int iSockID,int iBaudrate,
                          int iProtocolType);
    int SamCommand(int iSockID, const quint8 *sCommand, int iLenCmd, quint8* sReply, int &iLenRep);
    int GetReaderLastError(char *sLastError,int iMaxLength);
    void GetReaderVersion(char *sReaderVersion,
                         int iRVerMaxLength,
                         char *sAPIVersion, int iAPIVerMaxLength);
public:
    // 执行Psam卡指令
    bool SamCmd(int nSockID, const quint8* pCmd,
                int nCmdLen,quint8* pReply,
                int &nRepLen,int& nErrorCode,
                char* szErrMsg, int nMaxErrMsgLen);
    // 执行Pro卡指令
    bool ProCmd(const quint8* pCmd, int nCmdLen,
                quint8* pReply,int &nRepLen,
                int &nErrorCode,char* szErrMsg,
                int nMaxErrMsgLen);
    // Pro卡复合交易
    bool ProCapp(int nSockID, TProCardConsumeInfo &ConsumeInfo,
                 quint32 dwMoney, const QDateTime& ConsumeTime,
                  quint8* pData);
    // Pro卡复合交易
    bool ProCappForOldCard(int nSockID, TProCardConsumeInfo &ConsumeInfo,
                           quint32 dwMoney, const QDateTime& ConsumeTime,
                           const quint8* pData);
    // 提取卡终端机编号
    bool GetPsamTermNo(int nSockID,quint8 *psamTermNo);
    // 修改Pro卡文件
    bool ProModifyFile(const quint8 *pModifyCmd,
                       quint16 wCmdLen, const quint8 *Mac2);
public:
    // 读身份卡信息
    bool ReadOpCard(TOpCardBasicInfo& OpCardBasicInfo,TOpCardAppInfo& OpCardAppInfo);
    //读身份卡，供前台dll使用
    bool ReadOpCardDll(TDLLOpCardBasicInfo& OpCardBasicInfo);
    // 写身份卡信息 供前台dll使用
    bool WriteOpCardDll(const TDLLOpCardBasicInfo& OpCardBasicInfo);
    // 写身份卡信息 供前台
    bool WriteOpCard(TOpCardBasicInfo& OpCardBasicInfo,TOpCardAppInfo& OpCardAppInfo,bool opWrite2001);

    // 读取卡盒标签卡
    bool ReadBoxCard(TBoxCardBasicInfo& BoxCardBasicInfo,TBoxCardAppInfo& BoxCardAppInfo);
    // 写卡盒标签卡
    bool WriteBoxCard(quint32 dwCardId, const TBoxCardAppInfo& BoxCardAppInfo);
    // 读通行卡信息
    bool ReadTollCard(TTollCardBasicInfo& TollCardBasicInfo,TCardTollInfo& CardTollInfo);
    // 读包交卡信息
    bool ReadMonthCard(TMonthCardBasicInfo& CardBasicInfo);
    // 读Pro卡信息
    bool ReadProCard(TProCardBasicInfo& ProCardBasicInfo,
                     quint32 &dwBalance,TCardTollInfo& CardTollInfo);
    // 写Pro卡信息(测试用)
    bool  WriteProCard(const TProCardBasicInfo& ProCardBasicInfo);
    // 写入入口信息（清除标识站信息）
    bool WriteEntryInfo(QString sCardId,TCardTollInfo& CardTollInfo);
    // 写入出口信息  - 仅对通行卡
    bool WriteExitInfo(QString sCardId,quint32 dwBoxNo);
    // 回写卡（入/出口用） - 将卡片写回第一次读入的状态
    bool WriteBack(QString sCardId);
    // 云通卡复合交易（同时写入出口信息）
    bool ProConsume(QString sCardId,quint32 dwMoney, const QDateTime& ConsumeTime,TProCardConsumeInfo& pConsumeInfo);
    // 云通卡传统消费(只支持国标卡，装在第2槽位)
    bool ProTraditionConsume(quint32 dwCardId,quint32 dwMoney,const QDateTime& ConsumeTime,TProCardConsumeInfo& ConsumeInfo);
public:
    bool IsMifareOneCard();
    // 返回最近一次读入的卡类型
    int GetCardType();
    // 返回最近一次读入的卡印刷号
    QString GetCardId();
    // 写通行信息 供前台
    bool WriteTollInfo(const TCardTollInfo& CardTollInfo);
    // 取随机数
    bool ProGenRandom(quint32& dwRandom);

//    //因金卡提供DLL访问内存越界，造成程序崩溃。使用此函数间接调用DLL
//    int  JT_ReadFile(int iComID,int sFileID,quint8 sKeyType,int iAddr,int iLength,quint8 * sReply);
    quint32 GetCardSerialID()
    {
        return m_dwLastCardId;
    }

private:
    // 卡读写器序号，用于标识是第几个卡读写器
    int m_nReaderId;
    // 卡读写器串口号
    int m_nComID;
    // PSAM卡槽是否已初始化
    bool m_bPsamInited[MAX_PSAM_SOCKET];
    // 最近一次读入的卡类型
    int  m_nLastCardType;
    // 最近一次读入的卡序列号
    quint32  m_dwLastCardId;
    // 最近一次读入的卡印刷号(每次打开卡片时会清空)
    QString m_sLastCardId;
    // 最近一次读入的逻辑卡号（M1卡）
    quint32 m_dwLastCardNo;
    // 0扇区数据
    quint8 m_Sector00[SECTOR_LEN];
    //CPU卡发行文件
    TPro0015Raw m_Pro0015Raw;
    // 通行信息写卡失败保护
    TTollInfoWriteFailureProtector m_TollInfoWriteFailureProtector;
    // PSAM卡终端机编号（每个卡读写器应该都不一样）
    quint8 m_PsamTermNo[2][6];
    // 最近一次扣款的交易类型
    quint8 m_bLastTransType;
    // 最近一次读入的鲁通卡用户卡号
    quint8 m_ProCardNo[8];
    // 最近一次读入的鲁通卡区域分散因子
    quint8 m_ProCardIssueId[8];
    // 最近一次扣款的用户卡交易序号(仅用于扣款失败)
    quint16 m_wLastConsumeCardSeq;
    // 最近一次扣款的终端交易序号(仅用于扣款失败)
    quint32 m_dwLastConsumeTermSeq;
    // 前台身份卡写密钥计算方式有差异，故添加身份卡标识
    bool m_OpWriteCard;

private:
    // 判断卡片是否是M1卡
    bool GetIsMifareOneCard(CARD_TYPE_PHYSICAL CardType);
    // 计算新密钥M1卡扇区密钥
    bool CalNewCardKey(quint8 blockNo,int sKeyType,
                       int iSockId,quint8 *sReply,int &length);
    // 写卡文件
    bool WriteCardFile(int iFileID,int iAddr,int iLength,
                       quint8 *sData);
    // 文件标识
    bool GetCardFileSectorInfo(int nFileID,int & nStartSec,int &nSecCount);
    // 读PRO卡
    bool ReadProFile(quint8 bFileId,quint8 bBgnPosition,quint8 bLenToRead,quint8 *pData,quint8 &bLenRead);
    // 写PRO卡(数据长度限240字节以内)
    bool WriteProBinFile(int nSockId,quint8 nFileId,bool bNeedExternalAuthorize,quint8 nBgnPosition, const quint8 *pData,int nDataLen);
    // 外部认证
    bool ExternalAuthorize(int nSockId);

private:
    bool InitOldPsam(int nSockId);
    bool InitNewPsam(int nSockId);
    // 初始化PSAM卡
    bool InitPsam();
    // 复位PSAM卡，并设置工作波特率
    bool ResetPsam(int nSockId,int nInitBaudrate,int nWorkBaudrate,int nProtocolType);
    // 读PSAM卡终端机编号
    bool ReadPsamTermNo(int nSockId,char *PsamTermNo);
    // 选择PSAM卡目录
    bool SelPsamCardFile(int nSockId,quint16 wFile);
    // 选择PRO卡目录
    bool SelProCardFile(quint16 wFile);
    // 读未加密扇区（0扇区）
    bool ReadPubSector(quint16 dwCardSerialId);
    // 读CPU卡中0015文件
    bool Read0015(quint32 dwCardSerialId);
    // 保存卡类型和卡序列号
    void SaveOpenCardInfo(int nCardType,quint32 dwCardSerialId);
    //判断SAM卡插槽合法性
    bool IsValidSockId(int nSockId);
    //记录SAM卡初始化结果
    void SetSockInited(int nSockId,bool bInited);
    bool GetSockInited(int nSockId);
    // 判断M1卡时老卡还是新卡
    virtual  bool GetIsNewMifareOneCard();
    // 判断M1卡时老卡还是新卡
    virtual  bool GetIsNewMifareOneCard(const char *SectorData);
    // 判断CPU卡时老卡还是新卡
    virtual  bool GetIsNewProCard();
    // 判断CPU卡时老卡还是新卡
    virtual bool GetIsNewProCard(const TPro0015Raw& Pro0015Raw);
    // 写入口信息（新CPU卡）
    bool WriteEntryInfo_NewProCard(const TCardTollInfo& CardTollInfo);
    // 写入口信息（旧CPU卡）测试用
    bool WriteEntryInfo_OldProCard(const TCardTollInfo& CardTollInfo);
    // 写入口信息（新M1卡）
    bool WriteEntryInfo_NewPassCard(const TCardTollInfo& CardTollInfo);
    // 写入口信息（旧M1卡）测试用
    //bool WriteEntryInfo_OldPassCard(const TCardTollInfo& CardTollInfo);
    // 写出口信息 (新M1卡)
    bool WriteExitInfo_NewPassCard(quint32 dwBoxNo);
    // 写出口信息 (旧M1卡)
    bool WriteExitInfo_OldPassCard(quint32 dwBoxNo);
    // 回写卡(新M1卡)（入/出口用） - 将卡片写回第一次读入的状态
    bool WriteBack_NewPassCard(QString sCardId);
    // 回写卡(旧M1卡)（入/出口用） - 将卡片写回第一次读入的状态
    bool WriteBack_OldPassCard(QString sCardId);
    // 回写卡(新CPU卡)（入/出口用） - 将卡片写回第一次读入的状态
    bool WriteBack_NewProCard(QString sCardId);
    // 回写卡(旧CPU卡)（入/出口用） - 将卡片写回第一次读入的状态
    bool WriteBack_OldProCard(QString sCardId);

    // 读新密钥通行卡信息
    bool ReadNewPassTollCard(TTollCardBasicInfo &TollCardBasicInfo,
                             TCardTollInfo &CardTollInfo);

    // 读新标签卡信息
    bool ReadNewBoxCard(TBoxCardBasicInfo &BoxCardBasicInfo,
                        TBoxCardAppInfo& BoxCardAppInfo);
    // 写新标签卡信息
    bool WriteNewBoxCard(const TBoxCardAppInfo &BoxCardAppInfo);
    // 读旧标签卡信息
    bool ReadOldBoxCard(TBoxCardBasicInfo &BoxCardBasicInfo,
                        TBoxCardAppInfo &BoxCardAppInfo);
    // 写旧标签卡信息
    bool WriteOldBoxCard(const TBoxCardAppInfo& BoxCardAppInfo);

    // 读新身份卡信息
    bool ReadNewOpCard(TOpCardBasicInfo &OpCardBasicInfo,
                       TOpCardAppInfo &OpCardAppInfo);
    // 写新身份卡信息
    virtual bool WriteNewOpCard(TOpCardBasicInfo &OpCardBasicInfo,
                                TOpCardAppInfo &OpCardAppInfo,
                                bool  opWrite2001);
    // 写新身份卡信息
    virtual bool WriteNewOpCard(const  TOpCardBasicInfo &OpCardBasicInfo);

    // 读旧身份卡信息(不支持旧卡)
    bool ReadOldOpCard(TOpCardBasicInfo &OpCardBasicInfo,
                       TOpCardAppInfo &OpCardAppInfo);

    // 读新CPU卡信息
    bool ReadNewProCard(TPro0015Raw &Pro0015Raw,
                        TProCardBasicInfo &ProCardBasicInfo,
                        quint32 &dwBalance,TCardTollInfo&CardTollInfo);

    //比较读写操作
    bool CompareTollCardInfoRaw(TTollCardTollInfoRaw& writeInfo, TTollCardTollInfoRaw& readInfo);
public:
    //开始监听是否有卡片
    void StartCardDetection();
    //停止监听是否有卡片
    void StopCardDetection();
    //获取线程
    CardDetectThread* getDetectThread()
    {
        return &m_detectThread;
    }

private:
    CardDetectThread m_detectThread;
    QMutex m_mutex;
private:
    Func_JT_OpenReader JT_OpenReader;
    Func_JT_CloseReader JT_CloseReader;
    Func_JT_OpenCard JT_OpenCard;
    Func_JT_CloseCard JT_CloseCard;
    Func_JT_LEDDisplay JT_LEDDisplay;
    Func_JT_AudioControl JT_AudioControl;
    Func_JT_ReadBlock JT_ReadBlock;
    Func_JT_WriteBlock JT_WriteBlock;
    Func_JT_WriteKey JT_WriteKey;
    Func_JT_CPUCCommand JT_CPUCCommand;
    Func_JT_SamReset JT_SamReset;
    Func_JT_ChangeSamBaudrate JT_ChangeSamBaudrate;
    Func_JT_SamCommand JT_SamCommand;
    Func_JT_ReadFile JT_ReadFile;
    Func_JT_WriteFile JT_WriteFile;
    Func_JT_GetLastError JT_GetLastError;
    Func_JT_ReaderVersion JT_ReaderVersion;
};

QString GetProCardTypeName(int nCardType);


#endif // QCARDREADER_H
