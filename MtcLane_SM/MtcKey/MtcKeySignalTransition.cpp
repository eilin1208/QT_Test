#include "MtcKeySignalTransition.h"
void SpecialKeySignalTransition::mtcKeyEventTest(const MtcKeyPropertity &keyPropertity, int *keyType)
{
    switch(m_nKeyType)
    {
    case KC_Number:

        if(keyPropertity.isNumKey() && keyPropertity.func == m_nKey)
        {
            *keyType = m_nKeyType;
        }
        break;
    case KC_Letter:
        if(keyPropertity.isLetterKey() && keyPropertity.func == m_nKey)
        {
            *keyType = m_nKeyType;
        }
        break;
    case KC_VehClass:
        if(keyPropertity.isLetterKey() && keyPropertity.func == m_nKey)
        {
            *keyType = m_nKeyType;
        }
        break;
    case KC_Func:
        if(keyPropertity.isFuncKey() && keyPropertity.func == m_nKey)
        {
            *keyType = m_nKeyType;
        }
        break;
    default:
        break;

    }
}
