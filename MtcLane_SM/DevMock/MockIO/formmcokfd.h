#ifndef FORMMCOKFD_H
#define FORMMCOKFD_H
#include "idecoder.h"
#include "idisplay.h"
#include <QWidget>
namespace Ui {
    class FormMcokFD;
}

class FormMcokFD : public IDisplay
{
    Q_OBJECT

public:
    explicit FormMcokFD(QWidget *parent = 0);
    ~FormMcokFD();
    virtual void GetData(QByteArray &data);
public slots:
    virtual void beginDisplay();

    virtual QByteArray getBuffer();

private:
    Ui::FormMcokFD *ui;
    QByteArray m_fd;
};

#endif // FORMMCOKFD_H
