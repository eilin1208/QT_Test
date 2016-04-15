#include "cardfilemgr.h"
#include <QFile>
#include <QDebug>
#include <QQueue>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QSettings>
#include <qendian.h>

QString CardFilePath = "./CardFile";
static QString ReadFileName = "";
int type = 0;
int JT_OpenReader(int iComID, const char *sParas)
{
    return 0;
}
int JT_CloseReader(int iComID)
{
    return 0;
}
// 返回值 0打开卡错误;1 通行卡; 3 云通卡;
int JT_OpenCard(int iComID, quint8 *sCardSer,int &iMaxLength)
{
    QDir dir;
    if(!dir.exists(CardFilePath))
    {
        if(!dir.mkpath(CardFilePath))
            return 0;
    }
    /*以下为加载卡默认信息*/
    QSettings CardFile("./CardFile.ini",QSettings::IniFormat);
    QString sGroupName = "M1";
    //卡序列号 通行与云通卡公用
    quint32 CardNum = qFromBigEndian(CardFile.value(sGroupName+"/cardSer").toInt());
    iMaxLength = qMin(8,CardFile.value(sGroupName+"/cardSer").toString().length());
    memcpy(sCardSer,(char*)&CardNum,iMaxLength);
    //加载卡基本信息--通行卡
    memset(&TollcardBasicInfo,0,sizeof(TTollCardBasicInfoRaw));
    quint8 bVersion = CardFile.value(sGroupName+"/bVersion").toInt();
    quint16 wNetworkId = CardFile.value(sGroupName+"/wNetworkId").toInt();
    quint8 bType = CardFile.value(sGroupName+"/bType").toInt();
    quint32 dwCardNum = CardFile.value(sGroupName+"/dwCardNum").toInt();
    quint32 dwStartTime = CardFile.value(sGroupName+"/dwStartTime").toInt();
    quint32 dwExpireTime = CardFile.value(sGroupName+"/dwExpireTime").toInt();
    quint8 bIssueKeyVer = CardFile.value(sGroupName+"/bIssueKeyVer").toInt();
    quint8 dwIssueSAM[6];
    int len3 = qMin(CardFile.value(sGroupName+"/dwIssueSAM").toByteArray().length(),(int)sizeof(dwIssueSAM));
    memcpy(dwIssueSAM,CardFile.value(sGroupName+"/dwIssueSAM").toByteArray().data(),len3);
    quint32 dwIssueOrg = CardFile.value(sGroupName+"/dwIssueOrg").toInt();
    quint32 dwIssueOp = CardFile.value(sGroupName+"/dwIssueOp").toInt();
    quint16 wCRC = 1;
    //赋值--通行卡基本信息
    TollcardBasicInfo.M1BasicInfo.bVersion = bVersion;
    TollcardBasicInfo.M1BasicInfo.wNetworkId = wNetworkId;
    TollcardBasicInfo.M1BasicInfo.bType = bType;
    TollcardBasicInfo.M1BasicInfo.dwCardNum = dwCardNum;
    TollcardBasicInfo.M1BasicInfo.dwStartTime = dwStartTime;
    TollcardBasicInfo.M1BasicInfo.dwExpireTime = dwExpireTime;
    TollcardBasicInfo.bIssueKeyVer = bIssueKeyVer;
    memcpy(TollcardBasicInfo.dwIssueSAM,CardFile.value(sGroupName+"/dwIssueSAM").toByteArray().data(),len3);
    TollcardBasicInfo.dwIssueOrg = dwIssueOrg;
    TollcardBasicInfo.dwIssueOp = dwIssueOp;
    TollcardBasicInfo.wCRC = wCRC;
    //加载卡基本信息---云通卡
    memset(&Pro15Raw,0,43);
    //19
    memset(&Pro19Raw,0,43);
    //A
    memset(&ProARaw,0,83);
    //09
    memset(&Pro09Raw,0,9);
    sGroupName = "ProCard";
    //15文件赋值
    QString IssueOrgPro = CardFile.value(sGroupName+"/IssueOrg").toString();
    quint8 bTypePro = CardFile.value(sGroupName+"/bType").toInt();
    quint8 bVersionPro = CardFile.value(sGroupName+"/bVersion").toInt();
    quint16 NetworkPro = CardFile.value(sGroupName+"/Network").toInt();
    quint32 CardNoPro = CardFile.value(sGroupName+"/CardNo").toInt();
    QString StartTimePro = CardFile.value(sGroupName+"/StartTime").toString();
    QString ExpireTimePro = CardFile.value(sGroupName+"/ExpireTime").toString();
    char szVehPlatePro[12];
    memset(szVehPlatePro,0,12);
    QString tmpPlate = CardFile.value(sGroupName+"/szVehPlate").toString();
    int len = qMin(tmpPlate.toLocal8Bit().size(),(int)sizeof(szVehPlatePro));
    memcpy(szVehPlatePro,tmpPlate.toLocal8Bit().data(),len);
    quint8 bUserTypePro = CardFile.value(sGroupName+"/bUserType").toInt();
    quint8 bDiscountPro = CardFile.value(sGroupName+"/bDiscount").toInt();
    quint8 bTP = CardFile.value(sGroupName+"/bVehtypeAndPlateColor").toInt();
    memcpy(Pro15Raw.IssueOrg,IssueOrgPro.toLocal8Bit(),qMin(8,IssueOrgPro.length()));
    Pro15Raw.bType = bTypePro;
    Pro15Raw.bVersion = bVersionPro;
    ll2bcd((char*)(&Pro15Raw.Network),2,NetworkPro);
    Hex2Raw(Pro15Raw.CardNo,QString::number(CardNoPro).toLocal8Bit(),qMin(8,QString::number(CardNoPro).length()));
    memcpy(Pro15Raw.StartTime,StartTimePro.toLocal8Bit(),qMin(4,StartTimePro.length()));
    memcpy(Pro15Raw.ExpireTime,ExpireTimePro.toLocal8Bit(),qMin(4,ExpireTimePro.length()));
    memcpy(Pro15Raw.szVehPlate,Utf8toGB2312(szVehPlatePro,len),len);
    Pro15Raw.bDiscount = bDiscountPro;
    Pro15Raw.bVehtypeAndPlateColor = bTP;
    Pro15Raw.bUserType = bUserTypePro;
    //19
    Pro19Raw.bCompoundAppType = 0xAA;
    Pro19Raw.bRecLen = 0x29;
    Pro19Raw.bAppLockFlag = 0x00;
    //A
    ProARaw.bFreeType = 1;
    /*以下为通过判断文件确定返回类型,读取最新文件*/
    QSettings LaneFile("./Lane.ini",QSettings::IniFormat);
    sGroupName = "LaneInfo";
    type = LaneFile.value(sGroupName+"/LaneType").toInt();
    qDebug()<<QObject::tr("type:%1").arg(type);
    //type 1 入口; 2 出口 入口只做写卡操作
    if(type == 1)
    {
        static bool isToll = false;
        if(isToll)
        {
            isToll = false;
            ReadFileName = "";
            return 1;
        }
        else
        {
            isToll = true;
            ReadFileName = "";
            return 3;
        }
    }
    //出口 只进行读卡
    //存在文件 读取最早的文件中的内容
    QStringList fileList;
    QDir fileDir;
    fileDir.setPath(CardFilePath);
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(fileList));
    //目录中不存在文件
    if(fileInfo->count()<=2)
    {
        return 0;
    }
    QString newTFileName = "0";
    QString newPFileName = "0";
    for(int i = 0;i<= fileInfo->count() -1;i++)
    {
        if(fileInfo->at(i).isFile())
        {
            if(newTFileName.toLongLong()<= fileInfo->at(i).fileName().toLongLong())
            {
                if(fileInfo->at(i).size() == 98)
                {
                    newTFileName = fileInfo->at(i).fileName();
                }
                if(fileInfo->at(i).size() == 48)
                {
                    newPFileName = fileInfo->at(i).fileName();
                }
            }
        }
    }
    if(newTFileName <= newPFileName)
    {
        ReadFileName = newPFileName;
        return 3;
    }
    else
    {
        ReadFileName = newTFileName;
        return 1;
    }

    //出口,获取最早的文件进行读取
    // fileInfo 默认会读取"."和".."目录 文件从第三个开始索引
    //    ReadFileName = fileInfo->at(3).fileName();
    //    qDebug()<<QObject::tr("读取最早文件%1").arg(ReadFileName);
    //    for(int i = 0;i<= fileInfo->count() -1;i++)
    //    {
    //        if(fileInfo->at(i).isFile())
    //        {
    //            if(ReadFileName.toLongLong()>= fileInfo->at(i).fileName().toLongLong())
    //            {
    //                ReadFileName = fileInfo->at(i).fileName().toLongLong();
    //            }
    //        }
    //    }
    //    QFile tmpFile;
    //    tmpFile.setFileName(CardFilePath+"/"+ReadFileName);
    //    if(tmpFile.size() == 98)
    //    {
    //        return 1;
    //    }
    //    if(tmpFile.size() == 48)
    //    {
    //        return 3;
    //    }
    //    else
    //    {
    //        return 0;
    //    }
}
int JT_CloseCard(int iComID)
{
    return 1;
}
int JT_LEDDisplay(int iComID, quint8 cRed, quint8 cGreen, quint8 cBlue)
{
    return 1;
}
int JT_AudioControl(int iComID, quint8 cTimes, quint8 cVoice)
{
    return 1;
}
int JT_ReadBlock(int iComID, int iKeyType, int iBlockn, const quint8 *sKey, quint8 *sReply)
{
    //回复信息内容未知
    quint8 tmpRep[16];
    memset((char*)tmpRep,0,16);
    tmpRep[0] = 0x0F;
    tmpRep[1] = 0x01;
    //Hex2Raw(sReply,(char*)tmpRep,16);
    memcpy((char*)sReply,(char*)tmpRep,16);
    return 0;
}
int JT_WriteBlock(int iComID, int ikeyType, int iBlockn, const quint8 *sKey, quint8 *sData)
{
    //只在写身份卡时用到
    return 0;
}
int JT_WriteKey(int iComID, int iKeyType, int iSec, const quint8 *sKey, const quint8 *sKeyData)
{
    return 1;
}
int JT_CPUCCommand(int iComID, const quint8 *sCommand, int iLenCmd, quint8 *sReply, int &iLenRep)
{
    if (iLenRep>2 && NULL!=sReply)
    {
        //memset(sReply,0,iLenRep);
        sReply[0]=0x90;
        sReply[1]=0x00;
        if(iLenCmd == 5)
        {
            //加载目录中的文件
            QStringList fileList;
            QDir fileDir;
            fileDir.setPath(CardFilePath);
            QFile tmpFile;
            QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(fileList));

            //19Raw
            if(sCommand[2] == 0x01)
            {
                //目录中不存在文件
                if(ReadFileName == "")
                {
                    memcpy(sReply+2,&Pro19Raw,43);
                    iLenRep = 45;
                    return 0;
                }
                else
                {
                    qDebug()<<QObject::tr("Pro FileName:%1").arg(ReadFileName);
                    tmpFile.setFileName(CardFilePath+"/"+ReadFileName);
                    if(!tmpFile.open(QIODevice::ReadWrite))
                    {
                        qDebug()<<QObject::tr("open to read erro").arg(tmpFile.errorString());
                        return 2;
                    }
                    char tmpRep[48];
                    quint64 readLen = tmpFile.read(tmpRep,48);
                    if(readLen != 48)
                    {
                        return 1;
                    }
                    TPro0012Raw nRaw;
                    memcpy(&nRaw,tmpRep+8,40);
                    nRaw.wNetworkId = Pro15Raw.Network;
                    memcpy(sReply+2,&Pro19Raw,3);
                    memcpy(sReply+5,&nRaw,40);
                    iLenRep = 45;
                    if(type == 2)
                    {
                        tmpFile.close();
                        tmpFile.remove();
                        qDebug()<<"remove ";
                    }
                    return 0;
                }
            }
            else
            {
                // 0015文件
                if(sCommand[2] - 0x80 == 0x15 )
                {
                    memcpy(sReply+2,&Pro15Raw,43);
                    iLenRep = 45;
                    return 0;
                }
                if(sCommand[2] - 0x80 == 0x0A)
                {
                    memcpy(sReply+2,&ProARaw,83);
                    iLenRep = 85;
                    return 0;
                }
                if(sCommand[2] - 0x80 == 0x09)
                {
                    memcpy(sReply+2,&Pro09Raw,9);
                    iLenRep = 11;
                    return 0;
                }
                //读余额
                if(sCommand[0] == 128 && sCommand[1] == 92)
                {
                    //余额
                    quint32  dw = qFromBigEndian(500);
                    memcpy(sReply+2,QString::number(dw).toLocal8Bit(),QString::number(dw).length());
                    iLenRep = QString::number(dw).length() + 2;
                    return 0;
                }
            }
        }
        if(iLenCmd == 7)
        {
            return 0;
        }
        //写卡时读0x09
        if(iLenCmd == 14)
        {
            qDebug()<<" write come in 0x09";
            memcpy(sReply+2,&Pro09Raw,9);
            iLenRep = 11;
            return 0;
        }
        //写卡 procapp
        if(iLenCmd == 17)
        {
            memset(sReply+2,0,iLenRep-2);
            return 0;
        }
        if(iLenCmd == 21)
        {
            memset(sReply+2,0,iLenRep-2);
            //iLenRep = 11;
            return 0;
        }
        if(iLenCmd == 48)
        {
            static quint32 lastTime = 0;
            TPro0012Raw tmpRaw;
            memcpy(&tmpRaw,sCommand+8,40);
            if(lastTime == tmpRaw.dwPassTime)
            {
                memset(sReply+2,0,iLenRep-2);
                return 0;
            }
            lastTime = tmpRaw.dwPassTime;
            //生12文件 19文件由固定信息加12文件组成
            QString tmpFileName ="";
            tmpFileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
            QFile tmpFile(CardFilePath+"/"+tmpFileName);
            //打开文件进行写入
            if(!tmpFile.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text))
            {
                qDebug()<<QObject::tr("open to write erro").arg(tmpFile.errorString());
                return 2;
            }
            char tmpData[iLenCmd];
            memcpy(tmpData,sCommand,iLenCmd);
            quint64 wirLen = tmpFile.write(tmpData,iLenCmd);
            if(wirLen == iLenCmd)
            {
                tmpFile.flush();
                qDebug()<<QObject::tr("write to file:%1").arg(tmpFile.fileName());
                tmpFile.close();
                memset(sReply+2,0,iLenRep-2);
                ReadFileName = tmpFileName;
                return 0;
            }
            tmpFile.close();
            memset(sReply+2,0,iLenRep-2);
            return 1;
        }
    }
    return 1;
}
int JT_SamReset(int iComID, int iSockID, int iBaudrate, int iProtocolType, quint8 *sReply, int &iLenRep)
{
    if (iLenRep>2 && NULL!=sReply)
    {
        memset(sReply, 0, iLenRep);
        sReply[0]=0x90;
        sReply[1]=0x00;
        return 0;
    }
    return 1;
}
int JT_ChangeSamBaudrate(int iComID, int iSockID, int iBaudrate, int iProtocolType)
{
    return 1;
}
int JT_SamCommand(int iComID, int iSockID, const quint8 *sCommand, int iLenCmd, quint8 *sReply, int &iLenRep)
{
    if (5==iLenCmd)
    {
        if (0==memcmp(sCommand, "\x00\xB0\x96\x00\x06", iLenCmd))
        {
            // 读终端机编号
            iLenRep=8;
            char szPsamTermNo[255];
            memset(szPsamTermNo, '1', sizeof(szPsamTermNo));
            //GetPrivateProfileString("Common", "PsamTermNo", "", szPsamTermNo, _countof(szPsamTermNo), CFG_FILE );
            quint8 baPsamTermNo[16];
            memset(baPsamTermNo, 0, sizeof(baPsamTermNo));
            Hex2Raw(baPsamTermNo, szPsamTermNo, 12);
            memcpy(sReply, baPsamTermNo, 6);
            memcpy(sReply+6, "\x90\x00", 2);
            return 0;
        }
    }
    if(iLenCmd == 41 || iLenCmd == 9)
    {
        memset(sReply, 0, iLenRep-2);
        sReply[iLenRep-2]=0x90;
        sReply[iLenRep-1]=0x00;
        return 0;
    }
    if (iLenRep>2 && NULL!=sReply)
    {
        memset(sReply, 0, iLenRep-2);
        sReply[iLenRep-2]=0x90;
        sReply[iLenRep-1]=0x00;
        return 0;
    }
    return -1;
}
int JT_ReadFile(int iComID, int sFileId, quint8 sKeyType, int iAddr, int iLength, quint8 *sReply)
{
    qDebug()<<"start read";
    //读取基本信息
    if(iLength == 3)
    {
        memcpy(sReply,(char*)&TollcardBasicInfo,48);
        return 0;
    }
    //iaddr 每个文件中的记录数
    TTollCardTollInfoRaw TollCardTollInfoRaw;
    static QQueue<TTollCardTollInfoRaw> fileQue;
    int RecordNum = 1;
    iLength = iLength*16;
    static int readCount = 0;
    //加载目录中的文件
    QStringList fileList;
    QDir fileDir;
    fileDir.setPath(CardFilePath);
    QFile tmpFile;
    QList<QFileInfo> *fileInfo = new QList<QFileInfo>(fileDir.entryInfoList(fileList));
    //目录中不存在文件
    //    if(fileInfo->count()<=2)
    //    {
    //        memset(sReply,0,iLength);
    //        return 0;
    //    }
    //    //存在文件 读取最新的文件中的内容
    //    QString newFileName = "0";
    //    for(int i = 0;i<= fileInfo->count() -1;i++)
    //    {
    //        if(fileInfo->at(i).isFile() && fileInfo->at(i).size() == 98*RecordNum )
    //        {
    //            if(newFileName.toLongLong()<= fileInfo->at(i).fileName().toLongLong())
    //            {
    //                newFileName = fileInfo->at(i).fileName();
    //            }
    //        }
    //    }
    if(ReadFileName == "")
    {
        //没有符合读取条件的文件
        memset(sReply,0,iLength);
        qDebug()<<QObject::tr("第%1次读取错误,不存在符合读取条件的文件,请写卡!").arg(readCount+1);
        return 0;
    }
    //    tmpFile.setFileName(CardFilePath+"/"+newFileName);
    //    qDebug()<<QObject::tr("fileName:%1").arg(CardFilePath+"/"+newFileName);
    qDebug()<<QObject::tr("Toll FileName:%1").arg(ReadFileName);
    tmpFile.setFileName(CardFilePath+"/"+ReadFileName);
    if(!tmpFile.open(QIODevice::ReadWrite))
    {
        qDebug()<<QObject::tr("open to read erro").arg(tmpFile.errorString());
        return 2;
    }
    //将读取的文件中的内容加载队列
    char tmpRep[(iLength+2)*RecordNum];
    quint64 readLen = tmpFile.read(tmpRep,(iLength+2)*RecordNum);
    if(readLen != (iLength+2)*RecordNum)
    {
        return 1;
    }
    memcpy(sReply,tmpRep+1,iLength);
    tmpFile.close();
    if(type == 2)
    {
        tmpFile.remove();
    }
    readCount++;
    return 0;
}
int JT_WriteFile(int iComID, int sFileID, quint8 sKeyType, int iAddr, int iLength, const quint8 *sData)
{
    //开始写通行卡信息  通行卡ilength=6；
    //判断是否是写入的同一条记录
    static quint32 lastTime = 0;
    TTollCardTollInfoRaw tmpRaw;
    iLength = iLength * 16;
    memcpy(&tmpRaw,(char*)sData,iLength);
    if(lastTime == tmpRaw.dwPassTime)
    {
        return 0;
    }
    lastTime = tmpRaw.dwPassTime;
    QString tmpFileName = "";
    tmpFileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");
    QFile tmpFile(CardFilePath+"/"+tmpFileName);
    //打开文件进行写入
    if(!tmpFile.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text))
    {
        qDebug()<<QObject::tr("open to write erro").arg(tmpFile.errorString());
        return 2;
    }
    char tmpData[iLength+2];
    tmpData[0] = '<';
    tmpData[iLength+1] = '>';
    memcpy(tmpData+1,sData,iLength);
    quint64 wirLen = tmpFile.write(tmpData,iLength+2);
    if(wirLen == iLength+2)
    {
        tmpFile.flush();
        tmpFile.close();
        qDebug()<<QObject::tr("write to file:%1").arg(tmpFile.fileName());
        ReadFileName = tmpFileName;
        return 0;
    }
    tmpFile.close();
    return 1;
}
int JT_GetLastError(int iComID, char *sLastError, int iMaxLength)
{
    return 0;
}
int JT_ReaderVersion(int iComID, char *sReaderVersion, int iRVerMaxLength, char *sAPIVersion, int iAPIVerMaxLength)
{
    return 0;
}
