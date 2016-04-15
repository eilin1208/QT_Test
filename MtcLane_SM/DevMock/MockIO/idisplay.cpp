#include "idisplay.h"
QMap<int, IDisplay*> m_dsp;

IDisplay::IDisplay(QWidget *parent) :
    QWidget(parent)
{
}

void addDisplay(int type, IDisplay* display)
{
    m_dsp.insert(type, display);
}

IDisplay* getDisplay(int type)
{
    return m_dsp.value(type);
}
