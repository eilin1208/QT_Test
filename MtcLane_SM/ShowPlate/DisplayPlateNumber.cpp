#include "DisplayPlateNumber.h"
#include "MtcKey/MtcKeyDef.h"

const int CHAR_NUM_PER_PAGE=8;//每页个数
const int MAX_PLATE_LEN = 100;//最大数字个数

/***************************************
       根据车型获取车牌颜色
***************************************/
int DisplayPlateNumber::GetDefaultVehPlateColorForVehClass(int nVehClass)
{
    switch (nVehClass)
    {
    case VC_Car1:
        return VP_COLOR_BLUE;
    case VC_Car3:
    case VC_Car4:
    case VC_Truck2:
    case VC_Truck3:
    case VC_Truck4:
    case VC_Truck5:
        return VP_COLOR_YELLOW;
    default:
        return VP_COLOR_NONE;
    }
}
/***************************************
      根据按键获取车牌颜色
***************************************/
int DisplayPlateNumber::GetVehPlateColorFromKey(int nValue)
{
    switch (nValue)
    {
    case KeyBlue:
        return VP_COLOR_BLUE;
    case KeyYellow:
        return VP_COLOR_YELLOW;
    case KeyWhite:
        return VP_COLOR_WHITE;
    case KeyBlack:
        return VP_COLOR_BLACK;
    default:
        return VP_COLOR_NONE;
    }
}
/******************************************
根据输入拼音查询可选车牌，放入list中
******************************************/
QStringList DisplayPlateNumber::DisplaySelect(QString nValue)
{
    InitList();
    resultLetter = nValue;
    //DB.QueryVehPlateChar(resultLetter.toLocal8Bit().toLower(),list);
    return list;
}
/***************
     初始化
****************/
void DisplayPlateNumber::InitList()
{
    list.clear();
    resultLetter.clear();
    tempResult.clear();
    currentPage = 0;
    countPlate = 1;
}

/*********************************************
                    读取配置文件
*********************************************/
QString DisplayPlateNumber::ReadCfg()
{
    //实际用
    //    QSysParamFile * sysInfo;
    //    QString platePinyin;
    //    if(getParamFile(&sysInfo, cfSysParaDic) == true)
    //    {
    //        QSysParamInfo info = sysInfo->GetSysParamInfo();
    //        platePinyin  =  info.m_sDeftVLP ;
    //    }
    //     return platePinyin;
    //测试用
    QString platePinyin="123";
//    QSysParamFile infoFile;
//    infoFile.InitParamFile();
//    bool isNew;
//    SParamFileHead cfgHead;
//    if(infoFile.LoadParamFile(infoFile.GetFileName(), cfgHead, isNew) == true)
//    {
//        QSysParamInfo info = infoFile.GetSysParamInfo();
//        platePinyin  =  info.m_sDeftVLP ;
//    }
    return platePinyin;
}

void DisplayPlateNumber::LetterEventProcess(int &cursorPosition,QString  &sSelect,QString  &sResult)
{
    if(sResult.length()== cursorPosition)
    {
        cursorPosition++;
    }
    resultLetter.append(keyPropertity.ascii);
    list = DisplaySelect(resultLetter);
    if(list.size()!=0)
    {
        tempResult.clear();
        for(int i=0;i<qMin(CHAR_NUM_PER_PAGE,list.size());i++)
        {
            tempResult.append(QString::number(i)+"."+list[i]);
        }
        sSelect = resultLetter.toLower()+":  "+tempResult;
    }
    else
    {
        if(resultLetter.mid(resultLetter.length()-2,1)==(QString)keyPropertity.ascii)
        {
            if(cursorPosition<sResult.length())
            {
                sResult = sResult.left(cursorPosition)+keyPropertity.ascii+sResult.mid(cursorPosition);
                cursorPosition++;
            }
            else
            {
                sResult = sResult +keyPropertity.ascii;
            }
            sSelect.clear();
        }
        else
        {
            sSelect.clear();
        }

        resultLetter.clear();
    }
}
void DisplayPlateNumber::NumKey(int &cursorPosition,char keyValue, int i,QString &sSelect, QString &sResult)
{
    resultLetter.clear();
    QString str1,str2,finallStr ;
    if(sSelect!="")
    {
        if(i>=0 && i<qMin(CHAR_NUM_PER_PAGE,list.size()) )
        {
            str1 = tempResult.left(tempResult.indexOf(QString::number(i+1)));
            str2 = str1.mid(str1.indexOf(QString::number(i)));
            finallStr =str2.mid(2,1);
            if(cursorPosition<sResult.length())
            {
                sResult = sResult.left(cursorPosition)+finallStr+sResult.mid(cursorPosition);
            }
            else
            {
                sResult = sResult+finallStr;
            }
            sSelect.clear();
            resultLetter.clear();
        }
    }
    else
    {
        if(countPlate < MAX_PLATE_LEN +1)
        {
            if(cursorPosition<sResult.length())
            {
                sResult = sResult.left(cursorPosition)+QString::number(i)+sResult.mid(cursorPosition);
            }
            else
            {
                countPlate++;
                sResult = sResult+QString::number(i);
            }
        }
    }
}
void DisplayPlateNumber::NumEventProcess(int &cursorPosition,int nValue, QString &sSelect, QString &sResult)
{
    NumKey(cursorPosition,keyPropertity.ascii,(int)keyPropertity.ascii-Key0,sSelect,sResult);
    cursorPosition++;
}

void DisplayPlateNumber::FuncEventProcess(int &cursorPosition,int nValue, QString &sSelect, QString &sResult,QString &color)
{
    switch(keyPropertity.func)
    {
    case KeyRight: KeyRightProcess(cursorPosition,sSelect,sResult); break;
    case KeyLeft: KeyLeftProcess(cursorPosition,sSelect,sResult);break;
    case KeyDel: KeyDelProcess(cursorPosition,sSelect,sResult);break;
    case KeyEsc:ClearResult(sSelect,sResult); break;
    case KeyConfirm:KeyConfirmProcess(); break;
    }
}

/*********************************************
                键盘按键处理
*********************************************/
bool DisplayPlateNumber::KeyEventProcess(int &cursorPosition,int nValue,QString  &sSelect,QString  &sResult,QString &color)
{
    container.findKey(nValue,keyPropertity);
    if(keyPropertity.isLetterKey())
    {
        LetterEventProcess(cursorPosition,sSelect,sResult);
    }
    else if(keyPropertity.isVehPlateColorKey())
    {
        color = keyPropertity.keyName;
        resultLetter.clear();
    }
    else if(keyPropertity.isNumKey())
    {
        NumEventProcess(cursorPosition,nValue,sSelect,sResult);
    }
    else if(keyPropertity.isFuncKey())
    {
        FuncEventProcess(cursorPosition,nValue,sSelect,sResult,color);
    }
    else
    {
    }
    return true;
}



void DisplayPlateNumber::KeyRightProcess(int &cursorPosition,QString &sSelect, QString &sResult)
{

    if(list.size()%CHAR_NUM_PER_PAGE == 0)
    {
        totalPage =    list.size()/CHAR_NUM_PER_PAGE;
    }
    else
    {
        totalPage =list.size()/CHAR_NUM_PER_PAGE +1;
    }
    if(sSelect!=NULL)
    {
        currentPage++;
        if(currentPage < totalPage)
        {
            sSelect.clear();
            tempResult.clear();
            int index  = currentPage * CHAR_NUM_PER_PAGE;
            for(int i  = 0 ; i < qMin(list.length() - index, CHAR_NUM_PER_PAGE); i++)
            {
                tempResult.append(QString::number(i) + "." + list[index + i ]);
            }
            sSelect = resultLetter.toLower()+":  "+tempResult;
        }
        else
        {
            currentPage = totalPage-1;
        }
    }
    else
    {
        cursorPosition ++;
    }
    if(sResult==NULL&&sSelect == NULL)
    {
        sResult = ReadCfg();
        cursorPosition = sResult.length();
    }
}

void DisplayPlateNumber::KeyLeftProcess(int &cursorPosition,QString &sSelect, QString &sResult)
{
    if(sSelect!=NULL)
    {
        sSelect.clear();
        tempResult.clear();
        currentPage--;
        if(currentPage<0)
        {
            currentPage = 0;
        }
        int index  = currentPage * CHAR_NUM_PER_PAGE;
        for(int i  = 0 ; i < qMin(list.length() - index, CHAR_NUM_PER_PAGE); i++)
        {
            tempResult.append(QString::number(i) + "." + list[index + i ]);
            sSelect = resultLetter.toLower()+":  "+tempResult;
        }
    }
    else
    {
        cursorPosition--;
    }
}

void DisplayPlateNumber::KeyDelProcess(int &cursorPosition,QString &sSelect, QString &sResult)
{
    currentPage = 0;
    if(sSelect!=NULL)
    {
        resultLetter = resultLetter.remove(resultLetter.length()-1,1);
        list = DisplaySelect(resultLetter.trimmed());
        if(list.size()!=0&& resultLetter!=NULL)
        {
            tempResult.clear();
            for(int i=0;i<qMin(CHAR_NUM_PER_PAGE,list.size());i++)
            {
                tempResult.append(QString::number(i)+"."+list[i]);
            }
            sSelect = resultLetter.toLower()+":  "+tempResult;
        }
        if(resultLetter == NULL)
        {
            sSelect.clear();
        }
    }
    else
    {
        if(sResult!=NULL&&cursorPosition!=0)
        {
            sResult = sResult.remove(cursorPosition-1,1);
            cursorPosition--;
        }
        else
        {
            resultLetter.clear();
            countPlate =1;
        }
    }

}

void DisplayPlateNumber::ClearResult(QString &sSelect, QString &sResult)
{
    if(sSelect!=NULL)
    {
        sSelect.clear();
    }
    else
    {
        sResult.clear();
        countPlate =1;
    }
    resultLetter.clear();
}

void DisplayPlateNumber::KeyConfirmProcess()
{
}

DisplayPlateNumber::DisplayPlateNumber()
{
    InitList();
}







