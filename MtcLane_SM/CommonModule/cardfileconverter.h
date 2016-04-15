#ifndef CARDFILECONVERTERUNIT_H
#define CARDFILECONVERTERUNIT_H
#include <QObject>
#include <QDateTime>
#include <QString>
#include "CardFileDef.h"
#include "LaneType.h"
#include "laneutils_new.h"
#include<QDateTimeEdit>

class TCardFileConverter
{
public:

    // 通行卡基本信息转换（原始->逻辑）
    bool TollCardBasicInfoRaw2TollCardBasicInfo( TTollCardBasicInfo &TollCardBasicInfo,
                                                 quint32 dwCardSerialId,
                                                 const TTollCardBasicInfoRaw& TollCardBasicInfoRaw);
    // 旧通行卡基本信息转换(原始->逻辑)
    bool Old_TollCardBasicInfoRaw2TollCardBasicInfo(TTollCardBasicInfo &TollCardBasicInfo,
                                                    quint32 dwCardSerialId,
                                                    const TOldTollCardBasicInfoRaw& TollCardBasicInfoRaw);
    // 旧包交卡基本信息转换(原始->逻辑)
    bool Old_MonthCardBasicInfoRaw2MonthCardBasicInfo(TMonthCardBasicInfo& TollCardBasicInfo,
                                                      quint32 dwCardSerialId,
                                                      const TOldTollCardBasicInfoRaw& TollCardBasicInfoRaw);
    // 旧包交卡包交信息文件转换（原始->逻辑）
    //bool MonthCardRaw2Info(TMonthCardBasicInfo & MonthCardInfo, const TOldTollCardTollInfo1002Raw& File1002 );
    // 通行卡收费信息文件转换（原始->逻辑）
    bool TollCardTollInfoRaw2CardTollInfo(TCardTollInfo & CardTollInfo, const TTollCardTollInfoRaw& TollCardTollInfoRaw);
    // 通行卡收费信息文件转换（逻辑->原始）
    bool  CardTollInfo2TollCardTollInfoRaw(TTollCardTollInfoRaw &TollCardTollInfoRaw,const TCardTollInfo& CardTollInfo);
    // 身份卡基本信息转换（原始->逻辑）01 02
    bool OpCardBasicInfoRaw2OpCardBasicInfo(TOpCardBasicInfo &OpCardBasicInfo,
                                            quint32 dwCardSerialId, const TOpCardBasicInfoRaw& OpCardBasicInfoRaw ,
                                            const TOpCardInfoRaw& OpCardInfoRaw ); //overload;
    bool OpCardBasicInfoRaw2OpCardBasicInfo(TOpCardBasicInfo& OpCardBasicInfo,
                                            quint32 dwCardSerialId,
                                            const TOpCardBasicInfoRaw& OpCardBasicInfoRaw);
    // 非现金卡联网收费信息转换（逻辑->原始）
    bool CardTollInfo2Pro0012Raw(TPro0012Raw & Pro0012Raw,const TCardTollInfo& CardTollInfo);
    // 用于DLL，逻辑结构不同
   bool DLLOpCardBasicInfoRaw2OpCardBasicInfo(TDLLOpCardBasicInfo & OpCardBasicInfo,
                                              quint32 dwCardSerialId,const  TOpCardBasicInfoRaw& OpCardBasicInfoRaw ,const  TOpCardInfoRaw& OpCardInfoRaw );
    // 用于DLL，逻辑结构不同
    bool DLLOpCardBasicInfoRaw2OpCardBasicInfo(TDLLOpCardBasicInfo & OpCardBasicInfo,
                                               quint32 dwCardSerialId,
                                               const TOpCardBasicInfoRaw& OpCardBasicInfoRaw);
    // 身份卡基本信息转换（逻辑->原始）02 由于01无权限写故未添加
    bool OpCardBasicInfoO2CardBasicInfoRaw(TOpCardInfoRaw&OpCardInfoRaw, const TOpCardBasicInfo& OpCardBasicInfo);
    // 身份卡基本信息转换（逻辑->原始）02 由于01无权限写故未添加  --用于DLL，逻辑结构不同
    bool DLLOpCardBasicInfoO2CardBasicInfoRaw(TOpCardInfoRaw &OpCardInfoRaw,const TDLLOpCardBasicInfo& OpCardBasicInfo);
    // 身份卡附属信息转换（原始->逻辑）03
    bool OpCardAppInfoRaw2OpCardAppInfo(TOpCardAppInfo& OpCardAppInfo,const TOpCardAppInfoRaw&  OpCardAppInfoRaw);
    // 身份卡附属信息转换（逻辑->原始，仅前台使用）03
   bool OpCardAppInfo2OpCardAppInfoRaw( TOpCardAppInfoRaw &OpCardAppInfoRaw,const TOpCardAppInfo& OpCardAppInfo);
    // 标签卡基本信息转换（原始->逻辑）1001
    bool BoxCardBasicInfoRaw2BoxCardBasicInfo(TBoxCardBasicInfo& BasicInfo,
                                              quint32 dwCardSerialId,const TBoxCardBasicInfoRaw& BasicInfoRaw );
    // 旧标签卡基本信息转换（原始->逻辑）1001
    bool Old_BoxCardBasicInfoRaw2BoxCardBasicInfo( TBoxCardBasicInfo &BasicInfo,
                                                   quint32 dwCardSerialId,
                                                   const TOldBoxCardBasicInfoRaw& BasicInfoRaw );

    // 标签卡附属信息转换（原始->逻辑）2001
    bool BoxCardAppInfoRaw2BoxCardAppInfo( TBoxCardAppInfo &AppInfo,quint32 dwCardNum,const TBoxCardAppInfoRaw& AppInfoRaw );
    // 旧标签卡附属信息转换（原始->逻辑）2001
    bool Old_BoxCardAppInfoRaw2BoxCardAppInfo(TBoxCardAppInfo &AppInfo,
                                              quint32 dwCardNum,
                                              const TOldTollCardTollInfo2001Raw& AppInfoRaw );

    // 标签卡附属信息转换（逻辑->原始）2001
    bool BoxCardAppInfo2BoxCardAppInfoRaw(TBoxCardAppInfoRaw &AppInfoRaw ,const TBoxCardAppInfo& AppInfo);

    // 旧非现金卡基本信息转换（原始->逻辑）
    bool  OldProBasicRaw2ProCardBasicInfo(TProCardBasicInfo & ProCardBasicInfo,
                                          quint32 dwCardSerialId,
                                          const TOldPro0015Raw& Pro0015Raw);
    // 旧非现金卡基本信息转换（原始->逻辑）
    bool  OldProBasicRaw2ProCardBasicInfo(TProCardBasicInfo & ProCardBasicInfo,const TOldPro0015Raw& Pro0015Raw);

    // 非现金卡基本信息转换（原始->逻辑）
    bool  ProBasicRaw2ProCardBasicInfo(TProCardBasicInfo&ProCardBasicInfo,
                                       quint32 dwCardSerialId,
                                       const TPro0015Raw& Pro0015Raw,
                                       const TPro000ARaw& Pro000ARaw);
    // 非现金卡基本信息转换（原始->逻辑）
    bool  ProBasicRaw2ProCardBasicInfo(TProCardBasicInfo& ProCardBasicInfo,const TPro0015Raw& Pro0015Raw);



    // 非现金卡基本信息转换（逻辑->原始）
    bool ProCardBasicInfo2ProBasicRaw(TPro0015Raw &Pro0015Raw,TPro000ARaw & Pro000ARaw,const TProCardBasicInfo& ProCardBasicInfo);
    // 旧非现金卡基本信息转换（逻辑->原始）
    bool Old_ProCardBasicInfo2ProBasicRaw(TOldPro0015Raw& Pro0015Raw, const TProCardBasicInfo&  ProCardBasicInfo);

    // 非现金卡联网收费信息转换（原始->逻辑）
    bool Pro0012Raw2CardTollInfo(TCardTollInfo & CardTollInfo, const TPro0012Raw&  Pro0012Raw, const TPro0009Raw& Pro0009Raw);
    // 旧非现金卡联网收费信息转换（原始->逻辑）
    //bool Old_Pro0012Raw2CardTollInfo(TCardTollInfo& CardTollInfo,
                                     //const TOldPro0012Raw&  Pro0012Raw);
    // 旧非现金卡联网收费信息转换（逻辑->原始）
    bool Old_CardTollInfo2Pro0012Raw(TOldPro0012Raw& Pro0012Raw,const TCardTollInfo& CardTollInfo);
public:
    void  MaintenanceAndbPassStatus_card2file(quint8 &bMaintenance, quint8 &bPassStatus, quint8 bMaintenanceAndbPassStatus);
    void MaintenanceAndbPassStatus_file2card(quint8 &MaintenanceAndbPassStatus,const quint8 bMaintenance, quint8 bPassStatus);
    // 标识站信息转换（原始->逻辑）
    void flagStaInfo_card2file(quint8 & bFlagStaCount, QByteArray &baFlagStaInfo, const QByteArray& bFlagStaInfoRaw);
    // 标识站信息转换（逻辑->原始）
    bool  flagStaInfo_file2card(QByteArray&  bFlagStaInfoRaw,const quint8 bFlagStaCount, const QByteArray& baFlagStaInfo);
};
#endif // CARDFILECONVERTERUNIT_H
