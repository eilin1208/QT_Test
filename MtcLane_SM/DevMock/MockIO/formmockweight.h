#ifndef FORMMOCKWEIGHT_H
#define FORMMOCKWEIGHT_H
#include "idecoder.h"
#include "idisplay.h"
#include <QWidget>
typedef struct _CARINF
{
    char valid;
    char GMflag;
    int AxleNum;
    long AxleWt[50];
    int AxleGroupNum;
    long AxleGroupWt[50];
    int AxleGroupType[50];
    int  Speed;
public:
    void Init()
    {
        valid = '0';
        GMflag = '0';
        AxleNum = 0;
        AxleGroupNum = 0;
        Speed = -1;
        memset(AxleWt,0,50);
        memset(AxleGroupWt,0,50);
        memset(AxleGroupType,0,50);
    }
} CARINF, *pCARINF;
namespace Ui {
    class FormMockWeight;
}

class FormMockWeight : public IDisplay
{
    Q_OBJECT

public:
    explicit FormMockWeight(QWidget *parent = 0);

    ~FormMockWeight();

    virtual void GetData(QByteArray &data);

private slots:
    void on_push_send_clicked();

    void on_pushRand_clicked();

    void on_push_back_clicked();

    void on_push_clear_clicked();

public slots:
    virtual void beginDisplay();

    virtual QByteArray getBuffer();

private:
    Ui::FormMockWeight *ui;
    CARINF carinfo;
    QByteArray m_wt;
    int m_type;
};

#endif // FORMMOCKWEIGHT_H
