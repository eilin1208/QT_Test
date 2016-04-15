#ifndef FORMMOCKPRINTER_H
#define FORMMOCKPRINTER_H
#include "idecoder.h"
#include "idisplay.h"
#include <QWidget>
namespace Ui {
    class FormMockPrinter;
}

class FormMockPrinter : public IDisplay
{
    Q_OBJECT

public:
    explicit FormMockPrinter(QWidget *parent = 0);
    ~FormMockPrinter();
    virtual void GetData(QByteArray &data);
public slots:
    virtual void beginDisplay();
    virtual QByteArray getBuffer() ;

private:
    Ui::FormMockPrinter *ui;
    QByteArray m_prt;
};
#endif // FORMMOCKPRINTER_H
