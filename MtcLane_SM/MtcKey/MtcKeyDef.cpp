#include <QKeyEvent>
#include <QCoreApplication>
#include "MtcKeyDef.h"
#include "MtcKeySignalTransition.h"
QMap<int, MtcKeyPropertity> MtcKeyContainer::m_mapKeyPropertity;

const QEvent::Type MtcKeyPressedEvent::MtcKeyPressedEventType =
        static_cast<QEvent::Type>(QEvent::registerEventType());

MtcKeyPressedEvent::MtcKeyPressedEvent(const MtcKeyPropertity &value) : QEvent(MtcKeyPressedEventType)
{
    m_keyPropertity = value;
    m_keyType = KC_NONE;
}

int MtcKeyPropertity::getLogicKeyForNumKey() const
{
    return this->ascii-'0'+Key0;
}

int MtcKeyPropertity::getLogicKeyForLetterKey() const
{
    return this->ascii-'A'+KeyA;
}

int MtcKeyPropertity::getLogicKeyForFuncKey() const
{
    return this->func;
}

int MtcKeyPropertity::getLogicKeyForVehClassKey() const
{
    if (this->vehType>0 && this->vehType<=4)
    {
        return KeyCar1+this->vehType-1;
    }
    return this->func;
}

int MtcKeyPropertity::getLogicKey(int keyType) const
{
    switch (keyType)
    {
    case KC_NONE:
        break;
    case KC_Number:
        if (isNumKey())
        {
            return this->getLogicKeyForNumKey();
        }
        break;
    case KC_Letter:
        if (isLetterKey())
        {
            return this->getLogicKeyForLetterKey();
        }
        break;
    case KC_VehClass:
        if (isVehTypeKey())
        {
            return this->getLogicKeyForVehClassKey();
        }
        break;
    case KC_Func:
        if (this->isFuncKey())
        {
            return this->getLogicKeyForFuncKey();
        }
        break;
    }
    // 为无效按键（或者程序设置了错误的按键类型），为便于向后台发送按键键值，按照优先级返回键值，而非返回0
    if (isNumKey())
    {
        return this->getLogicKeyForNumKey();
    }
    else if (isLetterKey())
    {
        return this->getLogicKeyForLetterKey();
    }
    else if (isVehTypeKey())
    {
        return this->getLogicKeyForVehClassKey();
    }
    else if (this->isFuncKey())
    {
        return this->getLogicKeyForFuncKey();
    }
    else
    {
        // Assert!
        return 0;
    }
}

QString MtcKeyPropertity::getLogicKeyName(int keyType) const
{
    switch (keyType)
    {
    case KC_NONE:
        break;
    case KC_Number:
        if (isNumKey())
        {
            return this->keyName;
        }
        break;
    case KC_Letter:
        if (isLetterKey())
        {
            char buf[8];
            buf[0]=this->ascii;
            buf[1]=0;
            return buf;
        }
        break;
    case KC_VehClass:
        if (isVehTypeKey())
        {
            if (this->vehType>0 && this->vehType<=4)
            {
                QString s=QString::fromUtf8("客%1").arg(this->vehType);
                return s;
            }
            return this->keyName;
        }
        break;
    case KC_Func:
        if (this->func>0)
        {
            return this->keyName;
        }
        break;
    }
    // 为无效按键（或者程序设置了错误的按键类型），为便于日志记录和错误提示，返回默认的按键名称，
    //  而非QString::fromUtf8("无效键")
    return this->keyName;
}

MtcKeyProcessResultRecorder *MtcKeyProcessResultRecorder::m_instance = 0;

MtcKeyProcessResultRecorder::MtcKeyProcessResultRecorder() : QObject(qApp)
{
    m_bHasKeyWaitingToEmit=false;

    m_pTimer=new QTimer(this);
    m_pTimer->setSingleShot(true);
    m_pTimer->setInterval(15);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(sendRejectedKeyResult()));
}

void MtcKeyProcessResultRecorder::doKeyProcessed(const MtcKeyPropertity &keyPropertity, int keyType)
{
    if (m_bHasKeyWaitingToEmit)
    {
        if (m_mtcKeyWaitingToEmit.serialId==keyPropertity.serialId) // 同一按键
        {
            if (KC_NONE!=keyType)
            {
                // 按键被接受，直接发出信号
                m_bHasKeyWaitingToEmit=false;
                m_pTimer->stop();
                recordKeyProcessResult(keyPropertity, keyType);
            }
            else
            {
                // 同一按键再次无效，不用继续处理
            }
            // 已处理完毕，不再继续处理
            return;
        }
        else    // 不同的按键
        {
            // 不同的按键，将失败发出
            m_bHasKeyWaitingToEmit=false;
            m_pTimer->stop();
            recordKeyProcessResult(m_mtcKeyWaitingToEmit, KC_NONE);
            // 新的按键继续处理
        }
    }
    // 新的按键处理结果
    if (KC_NONE!=keyType)
    {
        // 按键被接受，直接发出信号
        recordKeyProcessResult(keyPropertity, keyType);
    }
    else
    {
        m_mtcKeyWaitingToEmit=keyPropertity;
        m_bHasKeyWaitingToEmit=true;
        m_pTimer->start();
    }
}

void MtcKeyProcessResultRecorder::sendRejectedKeyResult()
{
    if (m_bHasKeyWaitingToEmit)
    {
        m_bHasKeyWaitingToEmit=false;
        recordKeyProcessResult(m_mtcKeyWaitingToEmit, KC_NONE);
    }
    else
    {
        //QDebug() << "Assert Error in MtcKeyResultProcessor::sendRejectedKeyResult()";
    }
}

void MtcKeyProcessResultRecorder::recordKeyProcessResult(const MtcKeyPropertity &keyPropertity, int keyType)
{
    MtcKeyPressRecord rec;
    rec.logicKey=keyPropertity.getLogicKey(keyType);
    rec.pressTime=keyPropertity.pressTime;
    rec.isAccepted=(KC_NONE==keyType)?'0':'1';
    m_keyList.push_back(rec);
    if (m_keyList.size()>350)
    {
        m_keyList.pop_front();
    }
    emit keyProcessed(keyPropertity, keyType);
}

// 取出按键记录，并将已取出按键记录删除
//  输入：keyCount为最大的按键数，keyPressHistory指向的空间应大于keyCount*9
//  输出：keyCount为实际取出的按键数，每个按键记录的输出格式为xxhhnnssc，其中xx为逻辑按键键值的十六进制形式；
//      hhnnss为按键时、分、秒；c为0或1，表示按键是否被接受。
void MtcKeyProcessResultRecorder::getAndEraseKeyPressHistory(char *keyPressHistory, int &keyCount)
{
    int targetKeyCount=qMin(m_keyList.size(), keyCount);
    if (targetKeyCount<=0)
    {
        keyCount=0;
        return;
    }

    int nIndex=0;
    QList<MtcKeyPressRecord>::iterator it = m_keyList.begin();
    //    for(; (it!=m_keyList.end()) && nIndex<targetKeyCount; nIndex++)
    //    {
    //        char szBuff[16];
    //        sprintf(szBuff, "%.2x%.2d%.2d%.2d%c",
    //                it->logicKey, it->pressTime.hour(), it->pressTime.minute(), it->pressTime.second(), it->isAccepted);
    //        memcpy(keyPressHistory+9*nIndex,szBuff,9);
    //        it=m_keyList.erase(it);
    //    }
    //todo YN键值为8位,kkhhmmss
    for(; (it!=m_keyList.end()) && nIndex<targetKeyCount; nIndex++)
    {
        char szBuff[16];
        sprintf(szBuff, "%.2x%.2d%.2d%.2d",
                it->logicKey, it->pressTime.hour(), it->pressTime.minute(), it->pressTime.second());
        memcpy(keyPressHistory+8*nIndex,szBuff,8);
        it=m_keyList.erase(it);
    }
    keyCount=nIndex;
}

MtcKeyFilter *MtcKeyFilter::m_instance = 0;

MtcKeyFilter::MtcKeyFilter() : QObject(qApp)
{
}

bool MtcKeyFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        MtcKeyPropertity keyPropertity;
        if (!MtcKeyContainer::findKey(keyEvent->key(), keyPropertity))
        {
            LogMsg("key", tr("操作员按下非MTCLane按键，代码：0x%1").arg(keyEvent->key(), 0, 16));
        }
        else
        {
            static int STATIC_KEY_SERIAL_ID=0;
            keyPropertity.serialId=(++STATIC_KEY_SERIAL_ID);
            keyPropertity.pressTime.start();
            MtcKeyPressedEvent mtcKeyEvent(keyPropertity);
            bool bRlt=QCoreApplication::sendEvent(obj, &mtcKeyEvent);
            //QDebug() << "Mtc key" << mtcKeyEvent.key() << "(" << mtcKeyEvent.keyName().toAscii()
            //<< ") processed=" << mtcKeyEvent.processed() << "result=" << bRlt;
            LogMsg("key", tr("操作员按下MTCLane按键，按键：%1").arg(mtcKeyEvent.keyName()));
            if (mtcKeyEvent.processed())
            {
                // 按键事件被界面处理，发送按键处理成功信号
                MtcKeyProcessResultRecorder::instance()->doKeyProcessed(keyPropertity, mtcKeyEvent.getKeyType());
            }
            else
            {
                // 按键事件未被界面处理，发出信号，由状态机处理
                //QDebug() << "Emit key" << mtcKeyEvent.key();
                emit keyPressed(keyPropertity);
            }
        }
        return true;
    }

//    if (
//            (event->type() == QEvent::MouseButtonPress) ||
//            (event->type() == QEvent::MouseButtonRelease) ||
//            (event->type() == QEvent::MouseButtonDblClick) ||
//            (event->type() == QEvent::MouseMove) ||
//            (event->type() == QEvent::Enter) ||
//            (event->type() == QEvent::Leave) ||
//            (event->type() == QEvent::MouseTrackingChange))
//    {
//        if(event->type() == QEvent::MouseButtonPress)
//        {
//            LogMsg("key",tr("点击鼠标事件！X:%1,Y:%2")
//                   .arg(((QMouseEvent*)event)->globalX())
//                   .arg(((QMouseEvent*)event)->globalY()));
//        }
//        event->ignore();
//        return true;
//    }
    else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
#ifdef SDLane
// 山东省高速公路2010年键盘定义
void initKeyDef_SD2010()
{
    MtcKeyContainer::addKeyDef(Qt::Key_Escape,    27, ' ', KeyEsc,            -1, -1, QString::fromUtf8("取消"));
    MtcKeyContainer::addKeyDef(Qt::Key_Equal,    187, ' ', KeyCanopyLight,    -1, -1, QString::fromUtf8("雨棚开关"));
    MtcKeyContainer::addKeyDef(Qt::Key_PageUp,    33, ' ', KeyShift,          -1, -1, QString::fromUtf8("上/下班"));
    MtcKeyContainer::addKeyDef(Qt::Key_PageDown,  34, ' ', KeyViolation,      -1, -1, QString::fromUtf8("违章"));

    MtcKeyContainer::addKeyDef(Qt::Key_A,         65, 'A', KeyQuery,          -1, -1, QString::fromUtf8("入口图像"));
    MtcKeyContainer::addKeyDef(Qt::Key_B,         66, 'B', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_C,         67, 'C', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_D,         68, 'D', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_E,         69, 'E', KeyRepay,          -1, -1, QString::fromUtf8("补费"));
    MtcKeyContainer::addKeyDef(Qt::Key_F,         70, 'F', KeyTruck5,         15, -1, QString::fromUtf8("货5"));
    MtcKeyContainer::addKeyDef(Qt::Key_G,         71, 'G', KeyLockBar,        -1, -1, QString::fromUtf8("锁杆"));
    MtcKeyContainer::addKeyDef(Qt::Key_H,         72, 'H', KeyHaul,           -1, -1, QString::fromUtf8("牵引"));
    MtcKeyContainer::addKeyDef(Qt::Key_I,         73, 'I', KeyBadInvoice,     -1, -1, QString::fromUtf8("废票"));
    MtcKeyContainer::addKeyDef(Qt::Key_J,         74, 'J', KeyNoMoney,        -1, -1, QString::fromUtf8("欠费"));
    MtcKeyContainer::addKeyDef(Qt::Key_K,         75, 'K', KeyTruck4,         14, -1, QString::fromUtf8("货4"));
    MtcKeyContainer::addKeyDef(Qt::Key_L,         76, 'L', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_M,         77, 'M', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_N,         78, 'N', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_O,         79, 'O', KeyDetailWeight,   -1, -1, QString::fromUtf8("计重明细"));
    MtcKeyContainer::addKeyDef(Qt::Key_P,         80, 'P', KeyTruck3,         13, -1, QString::fromUtf8("货3"));
    MtcKeyContainer::addKeyDef(Qt::Key_Q,         81, 'Q', KeyPaperCard,      -1, -1, QString::fromUtf8("纸卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_R,         82, 'R', KeyBadCard,        -1, -1, QString::fromUtf8("坏卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_S,         83, 'S', KeyNoCard,         -1, -1, QString::fromUtf8("无卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_T,         84, 'T', KeyChangeCard,     -1, -1, QString::fromUtf8("换卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_U,         85, 'U', KeyTruck2,         12, -1, QString::fromUtf8("货2"));
    MtcKeyContainer::addKeyDef(Qt::Key_V,         86, 'V', KeyReset,          -1, -1, QString::fromUtf8("复位"));
    MtcKeyContainer::addKeyDef(Qt::Key_W,         87, 'W', KeyFarmProduct,    -1, -1, QString::fromUtf8("绿通"));
    MtcKeyContainer::addKeyDef(Qt::Key_X,         88, 'X', KeyOtherFree,      -1, -1, QString::fromUtf8("其它免征"));
    MtcKeyContainer::addKeyDef(Qt::Key_Y,         89, 'Y', KeyNormalVeh,      -1, -1, QString::fromUtf8("普通车"));
    MtcKeyContainer::addKeyDef(Qt::Key_Z,         90, 'Z', KeyTruck1,         11, -1, QString::fromUtf8("货1"));

    MtcKeyContainer::addKeyDef(Qt::Key_Home,      36, ' ', KeyInvoice,        -1, -1, QString::fromUtf8("票号"));
    MtcKeyContainer::addKeyDef(Qt::Key_End,       35, ' ', KeyFunc,           -1, -1, QString::fromUtf8("功能"));
    MtcKeyContainer::addKeyDef(Qt::Key_Asterisk, 106, ' ', KeyMotorcade,      -1, -1, QString::fromUtf8("车队"));
    for (int i=0; i<=9; i++)
    {
        MtcKeyContainer::addKeyDef(48+i, 48+i, char(48+i), -1, -1, -1);
    }
    MtcKeyContainer::addKeyDef(Qt::Key_Left,      37, ' ', KeyLeft,           -1, -1, QString::fromUtf8("←"));
    MtcKeyContainer::addKeyDef(Qt::Key_Up,        38, ' ', KeyUp,             -1, -1, QString::fromUtf8("↑"));
    MtcKeyContainer::addKeyDef(Qt::Key_Right,     39, ' ', KeyRight,          -1, -1, QString::fromUtf8("→"));
    MtcKeyContainer::addKeyDef(Qt::Key_Down,      40, ' ', KeyDown,           -1, -1, QString::fromUtf8("↓"));
    MtcKeyContainer::addKeyDef(Qt::Key_Period,   190, '.', KeyPoint,          -1, -1, QString::fromUtf8("."));
    MtcKeyContainer::addKeyDef(Qt::Key_F5,       116, ' ', KeyEditPlate,      -1, -1, QString::fromUtf8("更改车牌"));
    MtcKeyContainer::addKeyDef(Qt::Key_F1,       112, ' ', KeyBlue,            1,  0, QString::fromUtf8("蓝"));
    MtcKeyContainer::addKeyDef(Qt::Key_F2,       113, ' ', KeyYellow,          2,  1, QString::fromUtf8("黄"));
    MtcKeyContainer::addKeyDef(Qt::Key_F3,       114, ' ', KeyWhite,           3,  3, QString::fromUtf8("白"));
    MtcKeyContainer::addKeyDef(Qt::Key_F4,       115, ' ', KeyBlack,           4,  2, QString::fromUtf8("黑"));
    MtcKeyContainer::addKeyDef(Qt::Key_Insert,    45, ' ', KeyCash,           -1, -1, QString::fromUtf8("现金收讫"));

    MtcKeyContainer::addKeyDef(Qt::Key_Backspace,  8, ' ', KeyDel,            -1, -1, QString::fromUtf8("删除"));
    MtcKeyContainer::addKeyDef(Qt::Key_Slash,    111, ' ', KeyArmVeh,         -1, -1, QString::fromUtf8("军车"));
    MtcKeyContainer::addKeyDef(Qt::Key_Minus,    109, ' ', KeyOtherPayMethod, -1, -1, QString::fromUtf8("其它付款方式"));
    MtcKeyContainer::addKeyDef(Qt::Key_Plus,     107, ' ', KeyStoreCard,      -1, -1, QString::fromUtf8("鲁通卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_Return,     13, ' ', KeyConfirm,        -1, -1, QString::fromUtf8("确定"));
}
#endif
#ifdef YNLane

// NOTE:由于第二个addKeyDef中的第二个参数未使用到，因此在此传送的第二个参数值虽然不正确，但可以正常使用
void initKeyDef_YN2012()
{
    MtcKeyContainer::addKeyDef(Qt::Key_Escape,    27, ' ', KeyEsc,            -1, -1, QString::fromUtf8("取消"));
    MtcKeyContainer::addKeyDef(Qt::Key_F1,    187, ' ', KeyCanopyLight,    -1, -1, QString::fromUtf8("雨棚开关"));
    MtcKeyContainer::addKeyDef(Qt::Key_F2,    33, ' ', KeyShift,          -1, -1, QString::fromUtf8("上/下班"));
    MtcKeyContainer::addKeyDef(Qt::Key_F3,  34, ' ', KeyViolation,      -1, -1, QString::fromUtf8("违章"));
    MtcKeyContainer::addKeyDef(Qt::Key_F4,      36, ' ', KeyInvoice,        -1, -1, QString::fromUtf8("票号"));
    MtcKeyContainer::addKeyDef(Qt::Key_F5,       35, ' ', KeyFunc,           -1, -1, QString::fromUtf8("功能"));
    MtcKeyContainer::addKeyDef(Qt::Key_F6,       116, ' ', KeyEditPlate,      -1, -1, QString::fromUtf8("更改车牌"));
    MtcKeyContainer::addKeyDef(Qt::Key_F7,       112, ' ', KeyBlue,            -1,  0, QString::fromUtf8("蓝"));
    MtcKeyContainer::addKeyDef(Qt::Key_F8,       114, ' ', KeyYellow,           -1,  1, QString::fromUtf8("黄"));
    MtcKeyContainer::addKeyDef(Qt::Key_F9,       115, ' ', KeyBlack,           -1,  2, QString::fromUtf8("黑"));
    MtcKeyContainer::addKeyDef(Qt::Key_F10,       113, ' ', KeyWhite,          -1,  3, QString::fromUtf8("白"));

    MtcKeyContainer::addKeyDef(Qt::Key_F11,         79, ' ', KeyDetailWeight,   -1, -1, QString::fromUtf8("计重明细"));
    MtcKeyContainer::addKeyDef(Qt::Key_F12,         79, ' ', KeyAppExit,   -1, -1, QString::fromUtf8("程序退出"));

    MtcKeyContainer::addKeyDef(Qt::Key_A,         65, 'A', KeySimulate,                -1, -1, QString::fromUtf8("模拟"));
    MtcKeyContainer::addKeyDef(Qt::Key_B,         66, 'B', -1,                -1, -1);
    MtcKeyContainer::addKeyDef(Qt::Key_C,         67, 'C', KeyQuery,          -1, -1, QString::fromUtf8("入口图像"));
    MtcKeyContainer::addKeyDef(Qt::Key_D,         71, 'D', KeyLockBar,        -1, -1, QString::fromUtf8("锁杆"));
    MtcKeyContainer::addKeyDef(Qt::Key_E,         72, 'E', KeyHaul,           -1, -1, QString::fromUtf8("牵引"));
    MtcKeyContainer::addKeyDef(Qt::Key_F,         73, 'F', KeyBadInvoice,     -1, -1, QString::fromUtf8("废票"));
    MtcKeyContainer::addKeyDef(Qt::Key_G,         81, 'G', KeyPaperCard,      -1, -1, QString::fromUtf8("纸卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_H,         83, 'H', KeyNoCard,         -1, -1, QString::fromUtf8("无卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_I,         82, 'I', KeyBadCard,        -1, -1, QString::fromUtf8("坏卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_J,         84, 'J', KeyChangeCard,     -1, -1, QString::fromUtf8("换卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_K, 106, 'K', KeyMotorcade,      -1, -1, QString::fromUtf8("车队"));
    MtcKeyContainer::addKeyDef(Qt::Key_L, 106, 'L', KeyReversing,      -1, -1, QString::fromUtf8("倒车"));
    MtcKeyContainer::addKeyDef(Qt::Key_M,         87, 'M', KeyFarmProduct,    -1, -1, QString::fromUtf8("绿通"));
    MtcKeyContainer::addKeyDef(Qt::Key_N,         88, 'N', KeyOtherFree,      -1, -1, QString::fromUtf8("其它免征"));
    MtcKeyContainer::addKeyDef(Qt::Key_O,         88, 'O', KeyOffcial,      -1, -1, QString::fromUtf8("公务车"));
    MtcKeyContainer::addKeyDef(Qt::Key_P,    111, 'P', KeyArmVeh,         -1, -1, QString::fromUtf8("军警车"));
    MtcKeyContainer::addKeyDef(Qt::Key_Q,    111, 'Q', KeyMonthPass,         -1, -1, QString::fromUtf8("包交车"));
    MtcKeyContainer::addKeyDef(Qt::Key_R,    111, 'R', KeyCar1,         1, -1, QString::fromUtf8("一型客"));
    MtcKeyContainer::addKeyDef(Qt::Key_S,    111, 'S', KeyCar2,         2, -1, QString::fromUtf8("二型客"));
    MtcKeyContainer::addKeyDef(Qt::Key_T,    111, 'T', KeyCar3,         3, -1, QString::fromUtf8("三型客"));
    MtcKeyContainer::addKeyDef(Qt::Key_U,    111, 'U', KeyCar4,         4, -1, QString::fromUtf8("四型客"));
    MtcKeyContainer::addKeyDef(Qt::Key_V,         90, 'V', KeyTruck1,         11, -1, QString::fromUtf8("一型货"));
    MtcKeyContainer::addKeyDef(Qt::Key_W,         85, 'W', KeyTruck2,         12, -1, QString::fromUtf8("二型货"));
    MtcKeyContainer::addKeyDef(Qt::Key_X,         80, 'X', KeyTruck3,         13, -1, QString::fromUtf8("三型货"));
    MtcKeyContainer::addKeyDef(Qt::Key_Y,         75, 'Y', KeyTruck4,         14, -1, QString::fromUtf8("四型货"));
    MtcKeyContainer::addKeyDef(Qt::Key_Z,         70, 'Z', KeyTruck5,         15, -1, QString::fromUtf8("五型货"));
    MtcKeyContainer::addKeyDef(Qt::Key_1,         70, '1', Key1,         -1, -1, QString::fromUtf8("1"));
    MtcKeyContainer::addKeyDef(Qt::Key_2,         70, '2', Key2,         -1, -1, QString::fromUtf8("2"));
    MtcKeyContainer::addKeyDef(Qt::Key_3,         70, '3', Key3,         -1, -1, QString::fromUtf8("3"));
    MtcKeyContainer::addKeyDef(Qt::Key_4,         70, '4', Key4,         -1, -1, QString::fromUtf8("4"));
    MtcKeyContainer::addKeyDef(Qt::Key_5,         70, '5', Key5,         -1, -1, QString::fromUtf8("5"));
    MtcKeyContainer::addKeyDef(Qt::Key_6,         70, '6', Key6,         -1, -1, QString::fromUtf8("6"));
    MtcKeyContainer::addKeyDef(Qt::Key_7,         70, '7', Key7,         -1, -1, QString::fromUtf8("7"));
    MtcKeyContainer::addKeyDef(Qt::Key_8,         70, '8', Key8,         -1, -1, QString::fromUtf8("8"));
    MtcKeyContainer::addKeyDef(Qt::Key_9,         70, '9', Key9,         -1, -1, QString::fromUtf8("9"));
    MtcKeyContainer::addKeyDef(Qt::Key_0,         70, '0', Key0,         -1, -1, QString::fromUtf8("0"));

    //    MtcKeyContainer::addKeyDef(Qt::Key_C,         67, 'C', -1,                -1, -1);
    //    MtcKeyContainer::addKeyDef(Qt::Key_D,         68, 'D', -1,                -1, -1);
    //    MtcKeyContainer::addKeyDef(Qt::Key_E,         69, 'E', KeyRepay,          -1, -1, QString::fromUtf8("补费"));
    //    MtcKeyContainer::addKeyDef(Qt::Key_J,         74, 'J', KeyNoMoney,        -1, -1, QString::fromUtf8("欠费"));
    //    MtcKeyContainer::addKeyDef(Qt::Key_L,         76, 'L', -1,                -1, -1);
    //    MtcKeyContainer::addKeyDef(Qt::Key_M,         77, 'M', -1,                -1, -1);
    //    MtcKeyContainer::addKeyDef(Qt::Key_N,         78, 'N', -1,                -1, -1);

    //    MtcKeyContainer::addKeyDef(Qt::Key_V,         86, 'V', KeyReset,          -1, -1, QString::fromUtf8("复位"));
    MtcKeyContainer::addKeyDef(Qt::Key_Space,         89, ' ', KeyNormalVeh,      -1, -1, QString::fromUtf8("普通车"));
    //    for (int i=0; i<=9; i++)
    //    {
    //        MtcKeyContainer::addKeyDef(48+i, 48+i, char(48+i), -1, -1, -1);
    //    }
    MtcKeyContainer::addKeyDef(Qt::Key_Left,      37, ' ', KeyLeft,           -1, -1, QString::fromUtf8("←"));
    MtcKeyContainer::addKeyDef(Qt::Key_Up,        38, ' ', KeyUp,             -1, -1, QString::fromUtf8("↑"));
    MtcKeyContainer::addKeyDef(Qt::Key_Right,     39, ' ', KeyRight,          -1, -1, QString::fromUtf8("→"));
    MtcKeyContainer::addKeyDef(Qt::Key_Down,      40, ' ', KeyDown,           -1, -1, QString::fromUtf8("↓"));
    MtcKeyContainer::addKeyDef(Qt::Key_Period,   190, '.', KeyPoint,          -1, -1, QString::fromUtf8("."));
    MtcKeyContainer::addKeyDef(Qt::Key_Backslash,    45, ' ', KeyCash,           -1, -1, QString::fromUtf8("现金收讫"));
    MtcKeyContainer::addKeyDef(Qt::Key_Backspace,  8, ' ', KeyDel,            -1, -1, QString::fromUtf8("删除"));
    MtcKeyContainer::addKeyDef(Qt::Key_Comma,    109, ' ', KeyOtherPayMethod, -1, -1, QString::fromUtf8("其它付款方式"));
    MtcKeyContainer::addKeyDef(Qt::Key_Slash,     107, ' ', KeyStoreCard,      -1, -1, QString::fromUtf8("云通卡"));
    MtcKeyContainer::addKeyDef(Qt::Key_Return,     13, ' ', KeyConfirm,        -1, -1, QString::fromUtf8("确定"));
}
#endif
