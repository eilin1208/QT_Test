#include "formentryplate.h"
#include "ui_formentryplate.h"
#include "formmainwindow.h"
#include "CommonModule/LaneType.h"
#include "laneinfo.h"
#include "lanectrl.h"
#include "devicefactory.h"
#include "formvpr.h"
const char * STYLEBOLD = "border:8px solid;" \
        "border-top-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0.0903955 rgba(2, 136, 242, 255), stop:0.977401 rgba(84, 170, 245, 124));"\
        "border-bottom-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0.0903955 rgba(2, 136, 242, 255), stop:0.977401 rgba(84, 170, 245, 124));"\
        "border-left-color: qlineargradient(spread:pad, x1:1, y1:0, x2:0, y2:0, stop:0.0903955 rgba(2, 136, 242, 255), stop:0.977401 rgba(84, 170, 245, 124));"\
        "border-right-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0.0903955 rgba(2, 136, 242, 255), stop:0.977401 rgba(84, 170, 245, 124));";
FormEntryPlate::FormEntryPlate(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormEntryPlate)
{
    ui->setupUi(this);
    filterChildrenKeyEvent();
    FormVpr* pVpr = NULL;
    getWidgetByType(emFormVpr, (QWidget**)&pVpr);
    //2号选字框
    ui->labCharacter2->setVisible(false);
    m_nIndex = 0;
    connect(pVpr, SIGNAL(onGetVpr(int)), this, SLOT(getVprResult(int)));
}

FormEntryPlate::~FormEntryPlate()
{
    ////qDebug() << "release ....... " << objectName();
    delete ui;
}


void FormEntryPlate::getVprResult(int nIndex)
{
    if(!getLaneInfo()->getUsePlate())
    {
        return;
    }
    const CVPRResult* pVpr = NULL;
    pVpr = getDeviceFactory()->getVpr()->GetCurrentResult();
    if(pVpr == NULL)
    {
        return;
    }
    if(pVpr->sVehPlate.length() != 0)
    {
        snprintf(getLaneCtrl()->getTollItem()->m_VehInfo.szAutoVehPlate,
                 MAX_VEHPLATE_LEN, "%s", Utf8toGB2312(pVpr->sVehPlate).data());
        //TODO:颜色
        if(this->isVisible() && !ui->textNewNum->text().isEmpty())
        {
            getLaneInfo()->setVehPlateColor(pVpr->nColor);

        }
        else
        {
            memcpy(getLaneCtrl()->getTollItem()->m_VehInfo.szVehPlate,
                   getLaneCtrl()->getTollItem()->m_VehInfo.szAutoVehPlate,
                   MAX_VEHPLATE_LEN + 1);
            ui->textNewNum->setText(pVpr->sVehPlate);
        }
    }
    getLaneCtrl()->getTollItem()->SendVehInputChange(VCT_VLP);
}

void FormEntryPlate::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    //设定光标位置
    ui->textNewNum->setFocus();
    showBold();
    //输入键
    if(mtcKeyEvent->isNumKey() || mtcKeyEvent->isLetterKey())
    {
        if(ui->textNewNum->text().length() < 10)
        {
            inputPlate(mtcKeyEvent);
        }
    }
    //功能键
    if(mtcKeyEvent->isFuncKey())
    {
        //liujian
        if(mtcKeyEvent->func() == KeyConfirm)
        {
            if(ui->textNewNum->text() == "")
                return;
            else
            {
                getLaneInfo()->setVehPlate(ui->textNewNum->text());
                mtcKeyEvent->setKeyType(KC_Func);
                setResult(1);
                return;
            }
        }
        //liujian 有车牌颜色时esc为清空颜色功能
        if(mtcKeyEvent->func() == KeyEsc)
        {
            if(ui->labColor->text()!="" || ui->textNewNum->text() !="")
            {
                ui->labColor->setText("");
                ui->textNewNum->setText("");
            }
            else
            {
                setResult(0);
                mtcKeyEvent->setKeyType(KC_Func);
                return;
            }
        }
        if(mtcKeyEvent->func() == KeyPoint && ui->textNewNum->text() =="")
        {
            getLaneInfo()->setVehPlateColor(255);
            getLaneInfo()->setVehPlate("");
            mtcKeyEvent->setKeyType(KC_Func);
            setResult(1);
            return;
        }
        //左右移动与删除
        moveandDelInput(mtcKeyEvent);
        //更改车牌颜色
        if(mtcKeyEvent->isVehPlateColorKey())
        {
            handleVehColorKey(mtcKeyEvent);
        }
    }
    //getLaneInfo()->setVehPlate(ui->textNewNum->text());
    mtcKeyEvent->setKeyType(KC_Func);
}

// 车牌输入功能
void FormEntryPlate::inputPlate(MtcKeyPressedEvent *mtcKeyEvent)
{
    //获取当前光标位置
    int curPos = ui->textNewNum->cursorPosition();
    m_inPut.append(mtcKeyEvent->ascii());
    ////qDebug()<<ui->labColor->text();
    if(mtcKeyEvent->isLetterKey())
    {
        if(m_inPut.length() == 2 &&(m_inPut.left(1) == m_inPut.right(1)))
        {            //ui->textNewNum->setText(ui->textNewNum->text().append(m_inPut.left(1)));
            ui->textNewNum->setText(ui->textNewNum->text().insert(ui->textNewNum->cursorPosition(),m_inPut.left(1)));
            ui->textNewNum->setCursorPosition(curPos+1);
            characterList.clear();
            m_inPut.clear();
            ui->labCharacter->clear();
            ui->labCharacter2->clear();
            return;
        }
    }
    if(mtcKeyEvent->isNumKey())
    {
        if(ui->labCharacter->text().length() == 0)
        {
            mtcKeyEvent->setKeyType(KC_Number);
            //ui->textNewNum->setText(ui->textNewNum->text().append(m_inPut));
            ui->textNewNum->setText(ui->textNewNum->text().insert(ui->textNewNum->cursorPosition(),m_inPut.left(1)));
            ui->textNewNum->setCursorPosition(curPos+1);
            characterList.clear();
            m_inPut.clear();
            return;
        }
        else
        {
            int select = mtcKeyEvent->getLogicKeyName().toInt();
            if(select > characterList.count() || select == 0)
                return;
            if(select == 0)
            {
                select = 10;
            }
            if(m_nIndex != 0)
            {
                select += 10;
            }
            if(select > characterList.count())
            {
                return;
            }
            //ui->textNewNum->setText(ui->textNewNum->text().append(characterList.at(select-1)));
            ui->textNewNum->setText(ui->textNewNum->text().insert(ui->textNewNum->cursorPosition(),characterList.at(select-1)));
            ui->textNewNum->setCursorPosition(curPos+1);
            characterList.clear();
            m_inPut.clear();
            ui->labCharacter->clear();
            ui->labCharacter2->clear();
            m_nIndex = 0;
            showBold();
        }
    }
    GetCharacter(m_inPut);
}
//车牌输入时的左右键移动与删除功能
void FormEntryPlate::moveandDelInput(MtcKeyPressedEvent *mtcKeyEvent)
{
    ////qDebug() << "处理车牌界面删除键";
    //左移动键
    if(mtcKeyEvent->func() == KeyLeft)
    {
        if(ui->textNewNum->text().length()<=0)
            return;
        ui->textNewNum->cursorBackward(false,1);
    }    //右移动键
    if(mtcKeyEvent->func() == KeyRight)
    {
        QFileInfo fileInfo1(qApp->applicationDirPath() + "/Lane.bak");
        QSettings settings(fileInfo1.fileName() , QSettings::IniFormat);
        QString a = "VPR";
        QString vprHotkeys = (settings.value(a + "/VPRHotKeys").toString());
        if(ui->textNewNum->text().length()<=0)
            //liujian
        {
            //liubo
            if(vprHotkeys.isEmpty())
            {
                vprHotkeys = "G";
            }
            ui->textNewNum->setText(tr("云").append(vprHotkeys));
            ui->textNewNum->cursorForward(false,2);
            //return;
        }


        ui->textNewNum->cursorForward(false,1);
    }    //删除
    if(mtcKeyEvent->func() == KeyDel)
    {
        ////qDebug() << "车牌输入界面长度为" << ui->textNewNum->text().length();
        if(ui->textNewNum->text().isEmpty() && ui->labCharacter->text() == "")
        {
            //liujian
            if(ui->labColor->text() != "")
                ui->labColor->setText("");
            else
                //强制抓拍
                ////qDebug() << "强制抓拍" << getDeviceFactory()->getVpr()->Capture();
                return;
        }

        if(m_inPut.length()>0)
        {
            m_inPut = m_inPut.left(m_inPut.length()-1);
//            ui->labCharacter2->setText("");
        }
        else
        {
            if(ui->textNewNum->text().length()>0 && ui->textNewNum->cursorPosition()>0)
            {                int curPosition = ui->textNewNum->cursorPosition();
                int textLength = ui->textNewNum->text().length();
                ////qDebug()<<curPosition<<textLength;
                QString newText = ui->textNewNum->text().left(curPosition-1)+ui->textNewNum->text().right(textLength-curPosition);
                //ui->textNewNum->setText(ui->textNewNum->text().left(ui->textNewNum->text().length()-1));
                ui->textNewNum->setText(newText);
                ui->textNewNum->setCursorPosition(curPosition-1);
            }
        }
        GetCharacter(m_inPut);
    }
    //如果字符较多，2号选字框响应上下键
    if(ui->labCharacter2->isVisible())
    {
        if(mtcKeyEvent->func() == KeyUp)
        {
            m_nIndex = 0;
            showBold();
        }
        if(mtcKeyEvent->func() == KeyDown)
        {
            m_nIndex = 1;
            showBold();
        }
    }
}



void FormEntryPlate::handleVehColorKey(MtcKeyPressedEvent *mtcKeyEvent)
{
    ui->labColor->setText(mtcKeyEvent->getLogicKeyName());
    getLaneInfo()->setVehPlateColor(mtcKeyEvent->vehPlateColor());
}

void FormEntryPlate::showLetterResult(const QStringList &stringList)
{
    ui->labCharacter->clear();
    QString tmp;
    for(int i = 0; i < stringList.size(); i++)
    {
        tmp.append(tr("%1.%2").arg(i+1).arg(stringList[i]));
    }
    ui->labCharacter->setText(tmp);
}

//输入汉字查询显示
void FormEntryPlate::GetCharacter(QString szPinYin)
{
    //每次查询前都要清空显示的label和存储搜索选项的list
    if(szPinYin == "")
    {
        ui->labCharacter->clear();
        ui->labCharacter2->clear();
        return;
    }
    ui->labCharacter->clear();
    ui->labCharacter2->clear();
    characterList.clear();
    //连接数据库查询
    m_db.QueryVehPlateChar(szPinYin,characterList);
    int count = characterList.count();
    QString allCharacter = "";
    QString allCharacter2 = "";
    if(count > 0)
    {
        QString temp;
        for(int i = 0;i < count;i++)
        {
            if(i < 10)
            {
                temp = QString::number((i + 1)% 10);
                allCharacter = allCharacter +temp +"："+characterList.at(i)+"  ";
            }
            else
            {
                temp = QString::number((i -10 + 1) % 10);
                allCharacter2 = allCharacter2 +temp+"："+characterList.at(i)+"  ";
            }
        }
        ui->labCharacter->setText(allCharacter);
        ui->labCharacter2->setText(allCharacter2);
        ui->labCharacter2->setVisible(allCharacter2.size() != 0);
    }
    else
    {
        if(m_inPut.length() >= 2)
        {
            m_inPut.clear();
            characterList.clear();
            ui->labCharacter->clear();
        }
    }
}

void FormEntryPlate::showEvent(QShowEvent *event)
{
    ui->labColor->setText(GetVehColorQString(getLaneInfo()->getVehPlateColor()));
    ui->textNewNum->setText(getLaneInfo()->getVehPlate());
    //qDebug()<<"show颜色:"<<ui->labColor->text();
    m_inPut.clear();
    characterList.clear();
    ui->labCharacter->clear();
}

void FormEntryPlate::showBold()
{
    if(m_nIndex == 0)
    {
        ui->labCharacter->setStyleSheet(STYLEBOLD);
        ui->labCharacter2->setStyleSheet("");
    }
    else
    {
        if(ui->labCharacter2->isVisible())
        {
            ui->labCharacter2->setStyleSheet(STYLEBOLD);
            ui->labCharacter->setStyleSheet("");
        }
    }
}
FormEntryPlate* getShowEnPlate(QWidget *widget, QLayout *layout)
{
    //static DialogShowProInfo proInfo(widget, layout);
    static FormEntryPlate* p = NULL;
    if(p == NULL)
    {
        p = new FormEntryPlate(widget, layout);
    }
    return p;
}

void FormEntryPlate::ResetShow()
{
    if(ui->labColor->text() == "")
        getLaneInfo()->setVehPlateColor(0);
    ui->textNewNum->setText(getLaneInfo()->getVehPlate());
    m_inPut.clear();
    characterList.clear();
    ui->labCharacter->clear();
    //ui->labCharacter->setVisible(false);
    ui->labCharacter2->setVisible(false);
}

