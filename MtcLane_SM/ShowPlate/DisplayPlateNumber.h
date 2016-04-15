#ifndef DISPLAYPLATENUMBER_H
#define DISPLAYPLATENUMBER_H
#include<QStringList>
#include<QLineEdit>
#include<CommonModule/LaneType.h>
//#include "LocalCfgDb/LocalCfgDb.h"
#include "MtcKey/MtcKeyDef.h"

class DisplayPlateNumber
{

public:
    DisplayPlateNumber();
public:
    /***************************************
           根据车型获取车牌颜色
    ***************************************/
    int GetDefaultVehPlateColorForVehClass(int nVehClass);


    /***************************************
          根据按键获取车牌颜色
    ***************************************/
    int GetVehPlateColorFromKey(int nValue);


    /***************
         初始化
    ****************/
    void InitList();


    /******************************************
    根据输入拼音查询可选车牌，放入list中
    ******************************************/
    QStringList DisplaySelect(QString nValue);


    /*********************************************
                        读取配置文件
    *********************************************/
    QString ReadCfg();


    /*********************************************
                    键盘按键处理
    *********************************************/
    bool KeyEventProcess(int &cursorPosition,int nValue,QString  &selStr,QString  &sResultStr,QString &color);
    void LetterEventProcess(int &cursorPosition,QString  &sSelect,QString  &sResult);
    void NumEventProcess(int &cursorPosition,int nValue,QString  &sSelect,QString  &sResult);
    void FuncEventProcess(int &cursorPosition,int nValue,QString  &sSelect,QString  &sResult,QString &color);
    void NumKey(int &cursorPosition,char keyValue,int i,QString &sSelect, QString &sResult);
    void KeyRightProcess(int &cursorPosition,QString &sSelect, QString &sResult);
    void KeyLeftProcess(int &cursorPosition,QString &sSelect, QString &sResult);
    void KeyDelProcess(int &cursorPosition,QString &sSelect, QString &sResult);
    void ClearResult(QString &sSelect, QString &sResult);
    void KeyConfirmProcess();
public:
    MtcKeyContainer container;
    MtcKeyPropertity keyPropertity;
    QString resultLetter;//输入的按键
    QString tempResult;//最终显示结果(0.鲁1.民)
    QStringList list;//数据库查询结果
    int currentPage;//当前页
    int totalPage;//总页数
    QString plateLen;//存放输入的车牌号
    int countPlate;
private:
    //QLocalDbCfg DB;
};


#endif // DISPLAYPLATENUMBER_H
