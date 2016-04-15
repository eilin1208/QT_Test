#ifndef FORMENTRYVEHINFO_H
#define FORMENTRYVEHINFO_H

#include <QWidget>
#include "cardstatedetecter.h"
#include "MtcKey/MtcOpWidget.h"
#include "WtArm/wtdatadefineunit.h"
#include "WtArm/wtdevunit.h"
#include "WtArm/wtdatamgrunit.h"
namespace Ui {
class FormEntryVehInfo;
}

class FormEntryVehInfo : public MtcOpWidget
{
    Q_OBJECT
signals:
    void detectCardSuccess();
public:
    explicit FormEntryVehInfo(QWidget *parent = 0, QLayout* layout = 0);
    virtual ~FormEntryVehInfo();
    void showResultMode(bool bRes);
public slots:
    void getVprResult(int nIndex);
    void detectCard(quint32 nReadId, int nType);
public:
    void setPlateInfo(int nColor, const QString& sPlate);
    bool hasEnterVehType();
    void setTollFee(int nFee, int nCardFee);
    //显示免收
    void setTollFreeFee(int nFree);
    //是否显示了输入信息页面
    bool IsResultMode();
    //车型信息是否输入完全
    bool hasFinishVehInfo();
    //获取车型
    int getVehType()
    {
        return m_nVehType;
    }
    //获取车牌
    void getVehPlate(int &nColor, QString& sPlate);

    //获取车类
    int getVehClass()
    {
        return m_nVehClass;
    }
    void setVehClass(const QString& sVehClass, int nVehClass = 0);
    void clearData();
    //停止翻卡检测
    void stopCardStateDetecter();
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *);
private:
    void handleVehTypeKey(MtcKeyPressedEvent *mtcKeyEvent);
    void handleVehClassKey(MtcKeyPressedEvent *mtcKeyEvent);
    void handleEscKey();
    void beginDetectCard();
private:
    Ui::FormEntryVehInfo *ui;
    bool m_bHasEnterVehType;
    int m_nVehType;
    int m_nVehClass;
    int m_nPlateColor;
    bool m_bResultMode;
    //是否为第一次输入车型
    bool m_bFirstInput;
    //翻卡检测器
    CardStateDetecter m_cardDetecter;
};

#endif // FORMENTRYVEHINFO_H
