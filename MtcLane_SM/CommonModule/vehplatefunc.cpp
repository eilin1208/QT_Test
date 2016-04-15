#include "vehplatefunc.h"
#include "laneutils_new.h"
#include "ilogmsg.h"

void ChangeVehPlate(QString& sVehPlate)
{
    //先将所有letter转化为*
    for(int i = 0; i < sVehPlate.length(); i++)
    {
        if(sVehPlate.at(i).isLetter())
        {
            sVehPlate[i] = QChar('*');
        }
    }
    //去除多余*
    while(sVehPlate.contains("**"))
    {
        sVehPlate = sVehPlate.replace("**", "*");
    }
}

//获取车牌号的长度(汉字按一个计算, 例如:鲁A12345 返回为7)
int GetVehPlateLen(const char *szVehPlate)
{
    QString vehPlate = GB2312toUtf8(szVehPlate);
    LogMsg("lane",QObject::tr("获取转码车牌：%1转码后长度%2").arg(vehPlate).arg(vehPlate.length()));
    return vehPlate.length();
}


bool CompareVehPlate(const char *szVehPlate1, const char *szVehPlate2)
{
    //将两个字符串存储到qstring
    QString sPlate1 = GB2312toUtf8(szVehPlate1);
    QString sPlate2 = GB2312toUtf8(szVehPlate2);
    //判断2个字符串是否存在*
    if(sPlate1.contains('*') || sPlate2.contains('*'))
    {
        //将二者全部转化成*数字形式
        ////qDebug() << "compare" << sPlate1 << sPlate2;
        ChangeVehPlate(sPlate1);
        ChangeVehPlate(sPlate2);
        ////qDebug() << "after change" << sPlate1 << sPlate2;
        return sPlate1.compare(sPlate2) == 0;
    }
    else
    {
        //如果不包含*
        if(sPlate1.length() > sPlate2.length())
        {
            return sPlate1.contains(sPlate2);
        }
        else
        {
            return sPlate2.contains(sPlate1);
        }
    }
}

const char *GetVehPlateColorName(int nVehPlateColor)
{
    switch(nVehPlateColor)
    {
    case VP_COLOR_BLUE:
        return "蓝";
    case VP_COLOR_YELLOW:
        return "黄";
    case VP_COLOR_BLACK:
        return "黑";
    case VP_COLOR_WHITE:
        return "白";
    default:
        return "";
    }
}

void GetFullVehPlate(const char *szVehPlate, int nColor, QString &strFullVehPlate)
{
    strFullVehPlate.sprintf("%s%s", GetVehPlateColorName(nColor), szVehPlate);
}


void FullVehPlate2OldInputVehPlate(char *szOldInputVehPlate, const char *szFullVehPlate)
{
    int i=0;
    int j=0;
    while(szFullVehPlate[i])
    {
        if ((szFullVehPlate[i] < '0')||(szFullVehPlate[i] > '9'))
        {
            szOldInputVehPlate[j]='*';
            if (szFullVehPlate[i]<0)
            {
                i++;
            }
        }
        else
        {
            szOldInputVehPlate[j]=szFullVehPlate[i];
        }
        i++;
        j++;
    }
    szOldInputVehPlate[j]='\0';
    if (j>5)
    {
        for (int index1=0;index1<5;index1++)
        {
            szOldInputVehPlate[index1]=szOldInputVehPlate[j-5];
            j++;
        }
        szOldInputVehPlate[5]='\0';
    }
}

void RemovePlateSpecChar(char *szDest, int nDestSize, const char *szSrc, int nMaxVehPlateLen/*=MAX_VEHPLATE_LEN*/)
{
    memcpy(szDest, szSrc, qMin(nDestSize, nMaxVehPlateLen));
    return;
    if (nDestSize<1 || nMaxVehPlateLen<1 || strlen(szSrc)<1)
    {
        return;
    }
    memset(szDest,0,nDestSize);
    const int nMaxLen=qMin(nDestSize,nMaxVehPlateLen+1);
    int i=0,j=0;
    char szTmp[3]="";
    memset(szTmp,0,3);
    while (szSrc[i])
    {
        memset(szTmp,0,sizeof szTmp);
        //为兼容旧系统,'*'作为正常字符
        if (('0'<=szSrc[i] && szSrc[i]<='9')
                || ('A'<=szSrc[i] && szSrc[i]<='Z')
                /*|| '*'==szSrc[i]*/)
        {
            szTmp[0]=szSrc[i];
        }
        else if ('a'<=szSrc[i] && szSrc[i]<='z')
        {
            //小写字母转为大写
            szTmp[0]=szSrc[i]-32;
        }
        else if (0>szSrc[i])
        {
            //去除中文符号
            if (-80<=szSrc[i])
            {
                memcpy(szTmp,&szSrc[i],2);
            }
            else
            {
                LogMsg("lane", QObject::tr("this is special char, the index is %1").arg(i));
                i+=2;
                continue;
            }
        }

        int n=strlen(szTmp);
        if (0<n)
        {
            if (j<nMaxLen-n)
            {
                memcpy(&szDest[j],szTmp,n);
                i+=n;
                j+=n;
            }
            else
            {
                break;
            }
        }
        else
        {
            i++;
        }
    }
}

int TransVehPlateColor(const QString &sColor)
{
    if (0==sColor.compare("蓝"))
    {
        return VP_COLOR_BLUE;
    }
    if (0==sColor.compare("黄"))
    {
        return VP_COLOR_YELLOW;
    }
    if (0==sColor.compare("白"))
    {
        return VP_COLOR_WHITE;
    }
    if (0==sColor.compare("黑"))
    {
        return VP_COLOR_BLACK;
    }
    return VP_COLOR_NONE;
}

// 将车牌识别结果按格式要求进行转换
bool ConvertGDWPlate(int &nColor, QString &sPlate, const QString &sFullPlate)
{
    int len = sFullPlate.length();
    if (len<2)
    {
        // 全车牌中没有颜色
        nColor=VP_COLOR_NONE;
        sPlate=sFullPlate;
        return true;
    }
    QString sColor=sFullPlate.left(2);
    nColor=TransVehPlateColor(sColor);
    if (VP_COLOR_NONE==nColor)
    {
        // 如果前2个字符不是颜色，假定颜色未识别出
        sPlate=sFullPlate;
        if (0==sPlate.compare("无车牌"))
        {
            // 高德威车牌识别设备在没有车牌号时会上送“无车牌”
            sPlate="";
        }
    }
    else
    {
        sPlate=sFullPlate.mid(2, sFullPlate.length()-2);
    }
    return true;
}
