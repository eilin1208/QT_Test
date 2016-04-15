#include "cardstatedetecter.h"

CardStateDetecter::CardStateDetecter(QObject *parent) :
    QObject(parent), m_nCardId(0), m_nCardType(0),m_bNoCard(false)
{
}

void CardStateDetecter::hasOneCard(quint32 nCardId, int nCardType)
{
    //第一次识别到卡
    if(m_nCardId == 0 && m_nCardType == 0)
    {
        m_nCardId = nCardId;
        m_nCardType = nCardType;
        m_bNoCard = false;
    }
    else if(m_bNoCard == true)
        //已经翻卡
    {
        if(m_nCardId == nCardId && m_nCardType == nCardType)
        {
            //模拟云通卡按键
            QCoreApplication::postEvent(QApplication::instance(), new QKeyEvent(QEvent::KeyPress,Qt::Key_Slash,Qt::NoModifier));
            QCoreApplication::postEvent(QApplication::instance(), new QKeyEvent(QEvent::KeyRelease, Qt::Key_Slash,Qt::NoModifier));
            Close();
        }
    }
}

void CardStateDetecter::hasNoCard()
{
    m_bNoCard = true;
}

void CardStateDetecter::Start()
{
    m_nCardId = 0;
    m_nCardType = 0;
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    pReader->StopCardDetection();
    connect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
            this, SLOT(hasOneCard(quint32,int)));
    connect(pReader->getDetectThread(), SIGNAL(hasNoCard()),
            this, SLOT(hasNoCard()));
    pReader->StartCardDetection();
}

void CardStateDetecter::Close()
{
    m_nCardId = 0;
    m_nCardType = 0;
    m_bNoCard = false;
    QCardReader* pReader = getDeviceFactory()->getCardReader(DEV_CardReader1);
    disconnect(pReader->getDetectThread(), SIGNAL(hasOneCard(quint32,int)),
            this, SLOT(hasOneCard(quint32,int)));
    disconnect(pReader->getDetectThread(), SIGNAL(hasNoCard()),
            this, SLOT(hasNoCard()));
    pReader->StopCardDetection();
}
