#ifndef MTC_KEY_DEF_H
#define MTC_KEY_DEF_H

#include <QEvent>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>
#include "CommonModule/ilogmsg.h"
enum KeyClassTypeDefine
{
    KC_NONE,
    KC_Number,          // 数值键
    KC_Letter,          // 字母键
    KC_VehClass,        // 车型键
    KC_Func             // 功能键
};

class MtcKeyPropertity
{
public:
    int serialId;       // 序列号（程序启动后从1开始累进）
    QTime pressTime;    // 按键时间
    int key;            // Qt键值
    char ascii;         // 'A'..'Z', '0'..'9'
    int func;           // invalid for negative value
    int vehType;        // invalid for negative value
    int vehPlateColor;  // invalid for negative value
    QString keyName;    // 按键名称
public:
    inline bool isNumKey() const
    {
        return (ascii >= '0') && (ascii <= '9');
    }

    inline bool isLetterKey() const
    {
        return (ascii >= 'A') && (ascii <= 'Z');
    }

    inline bool isFuncKey() const
    {
        return func>0;
    }

    inline bool isVehTypeKey() const
    {
        return vehType>0;
    }

    inline bool isVehPlateColorKey() const
    {
        return vehPlateColor>=0;
    }

    inline static bool processed(int keyType)
    {
        return keyType!=KC_NONE;
    }

    int getLogicKey(int keyType) const;

    QString getLogicKeyName(int keyType) const;

protected:
    int getLogicKeyForNumKey() const;
    int getLogicKeyForLetterKey() const;
    int getLogicKeyForFuncKey() const;
    int getLogicKeyForVehClassKey() const;
};
Q_DECLARE_METATYPE(MtcKeyPropertity);

// NOTE: 此处按键键值不能更改，车道向后台传输此按键键值

#ifdef YNLane
enum MtcFuncKeyValue
{
    KeyNull,
    Key0,           // Code=48
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
    Key9,
    KeyA,		// Code=65
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    keyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,
    KeyLeft,
    KeyDown,
    KeyUp,
    KeyRight,
    KeyPoint,           // "."
    KeyDel,           // 删除 BackSpace
    KeyEsc,          // 取消 Esc
    KeyConfirm,      // 回车 Enter
    KeyCanopyLight,     // 雨棚开关 =
    KeyShift,           // 上/下班 pg up
    KeyViolation,       // 违章	pg dn
    KeyInvoice,         // 票号Home
    KeyFunc,            // 功能 End
    KeyEditPlate,       // 更改车牌 F5
    KeyBlue,            // F1
    KeyWhite,           // F3
    KeyBlack,           // F4
    KeyYellow,         // F2
    KeyDetailWeight,    // 计重明细 O
    KeyAppExit, //程序退出
    KeyQuery,           // 入口图像A
    KeySimulate,                //模拟
    KeyMotorcade,       // 车队*
    KeyChangeCard,      // 换卡 T
    KeyLockBar,         // 锁杆 G
    KeyHaul,            // 牵引 H
    KeyBadInvoice,      // 废票 I
    KeyPaperCard,       // 纸卡 Q
    KeyNoCard,          // 无卡 S
    KeyBadCard,         // 坏卡 R
    KeyReversing,       // 倒车
    KeyFarmProduct,     // 绿通 W
    KeyOtherFree,       // 其它免征 X
    KeyNormalVeh,       // 普通车 Y
    KeyArmVeh,          // 军车 /
    KeyOffcial,         //公务车
    KeyMonthPass,//包交车，月票车
    KeyCar1,            // 客1
    KeyCar2,            // 客2
    KeyCar3,            // 客3
    KeyCar4,            // 客4
    KeyTruck1,          // Z
    KeyTruck2,          // U
    KeyTruck3,          // P
    KeyTruck4,          // K
    KeyTruck5,          // F
    KeyCash,            // 现金 Ins
    KeyStoreCard,       // 鲁通卡
    KeyOtherPayMethod   // -
};
#endif
class MtcKeyContainer
{
public:
    static bool addKeyDef(const MtcKeyPropertity &value)
    {
        m_mapKeyPropertity[value.key]=value;
        return true;
    }

    static bool addKeyDef(int key, int vk, char ascii, int func, int vehType, int vehPlateColor, const QString &keyName)
    {
        MtcKeyPropertity value;
        value.serialId=0;
        value.key=key;
        value.ascii=ascii;
        value.func=func;
        value.vehType=vehType;
        value.vehPlateColor=vehPlateColor;
        value.keyName=keyName;
        return addKeyDef(value);
    }

    static bool addKeyDef(int key, int vk, char ascii, int func, int vehType, int vehPlateColor)
    {
        char buf[8];
        buf[0]=ascii;
        buf[1]=0;
        return addKeyDef(key, vk, ascii, func, vehType, vehPlateColor, buf);
    }

    static bool findKey(int key, MtcKeyPropertity &value)
    {
        if (m_mapKeyPropertity.find(key)!=m_mapKeyPropertity.end())
        {
            value=m_mapKeyPropertity.value(key);
            return true;
        }
        return false;
    }

public:
    MtcKeyContainer()
    {
    }

protected:
    static QMap<int, MtcKeyPropertity> m_mapKeyPropertity;
};

class MtcKeyPressedEvent : public QEvent
{
public:
    MtcKeyPressedEvent(const MtcKeyPropertity &value);

    inline bool processed() const
    {
        return MtcKeyPropertity::processed(m_keyType);
    }

    inline KeyClassTypeDefine getKeyType() const
    {
        return m_keyType;
    }

    inline void setKeyType(KeyClassTypeDefine keyType)
    {
        m_keyType=keyType;
    }

    inline int key() const
    {
        return m_keyPropertity.key;
    }

    inline QString keyName() const
    {
        return m_keyPropertity.keyName;
    }

    inline bool isNumKey() const
    {
        return m_keyPropertity.isNumKey();
    }

    inline bool isLetterKey() const
    {
        return m_keyPropertity.isLetterKey();
    }

    inline char ascii() const
    {
        return m_keyPropertity.ascii;
    }

    inline bool isFuncKey() const
    {
        return m_keyPropertity.isFuncKey();
    }

    inline bool isVehClassKey() const
    {
        return ('M' <= m_keyPropertity.ascii) && (m_keyPropertity.ascii <= 'Q');
    }

    inline int func() const
    {
        return m_keyPropertity.func;
    }

    inline bool isVehTypeKey() const
    {
        return m_keyPropertity.isVehTypeKey();
    }

    inline int vehType() const
    {
        return m_keyPropertity.vehType;
    }

    inline bool isVehPlateColorKey() const
    {
        return m_keyPropertity.isVehPlateColorKey();
    }

    inline int vehPlateColor() const
    {
        return m_keyPropertity.vehPlateColor;
    }

    inline int getLogicKey() const
    {
        return m_keyPropertity.getLogicKey(m_keyType);
    }

    inline QString getLogicKeyName() const
    {
        return m_keyPropertity.getLogicKeyName(m_keyType);
    }
public:
    inline static Type getEventType()
    {
        return MtcKeyPressedEventType;
    }

protected:
    static const Type MtcKeyPressedEventType;

protected:
    MtcKeyPropertity m_keyPropertity;
    KeyClassTypeDefine m_keyType;
};

class MtcKeyPressRecord
{
public:
    int logicKey;       // 按键对应的逻辑键值（传递给后台的键值）
    quint8 isAccepted;  // 按键是否被接受处理
    QTime pressTime;    // 按键时间（按键时记录）
};

// 因为Transition没有返回值，为按键Transition提供信号发送机制
//  由于系统可能在多个Transition测试，直到遇到可以进行状态转换的Transition，为避免单个按键有多个返回值，
//  将返回失败的按键放在缓存中，过一段时间后再发送，滤过重复的错误值。
class MtcKeyProcessResultRecorder : public QObject
{
    Q_OBJECT

public:
    static MtcKeyProcessResultRecorder *instance()
    {
        if (!m_instance)
        {
            if (!m_instance)
            {
                m_instance = new MtcKeyProcessResultRecorder();
            }
        }

        return m_instance;
    }

private:
    MtcKeyProcessResultRecorder();
    static MtcKeyProcessResultRecorder *m_instance;

public:
    void doKeyProcessed(const MtcKeyPropertity &keyPropertity, int keyType);

public:
    // 取出按键记录，并将已取出按键记录删除
    //  输入：keyCount为最大的按键数，keyPressHistory指向的空间应大于keyCount*9
    //  输出：keyCount为实际取出的按键数，每个按键记录的输出格式为xxhhnnssc，其中xx为逻辑按键键值的十六进制形式；
    //      hhnnss为按键时、分、秒；c为0或1，表示按键是否被接受。
    void getAndEraseKeyPressHistory(char *keyPressHistory, int &keyCount);

private:
    void recordKeyProcessResult(const MtcKeyPropertity &keyPropertity, int keyType);

private slots:
    void sendRejectedKeyResult();

private:
    QList<MtcKeyPressRecord> m_keyList;
    // 失败按键缓存
    MtcKeyPropertity m_mtcKeyWaitingToEmit;
    bool m_bHasKeyWaitingToEmit;
    QTimer *m_pTimer;

signals:
    void keyProcessed(const MtcKeyPropertity &keyPropertity, int keyType);
};

class MtcKeyFilter : public QObject
{
    Q_OBJECT

public:
    static MtcKeyFilter *instance()
    {
        static QMutex mutex;
        if (!m_instance)
        {
            QMutexLocker locker(&mutex);
            if (!m_instance)
            {
                qRegisterMetaType<MtcKeyPropertity>("MtcKeyPropertity");
                m_instance = new MtcKeyFilter;
            }
        }

        return m_instance;
    }

private:
    MtcKeyFilter();
    static MtcKeyFilter *m_instance;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void keyPressed(const MtcKeyPropertity &keyPropertity);
};
#ifdef SDLane
// 山东省高速公路2010年键盘定义
void initKeyDef_SD2010();
#endif
#ifdef YNLane
// 云南省高速公路2012年键盘定义
void initKeyDef_YN2012();
#endif
#endif // MTC_KEY_DEF_H
