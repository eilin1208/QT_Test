#ifndef FORMMOCKIO_H
#define FORMMOCKIO_H
#include <QWidget>
#include "idecoder.h"
#include "idisplay.h"
 #include <QCloseEvent>
namespace Ui {
    class FormMockIO;
}
typedef struct {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
    unsigned int d:1;
    unsigned int e:1;
    unsigned int f:1;
    unsigned int g:1;
    unsigned int h:1;

    unsigned int i:1;
    unsigned int j:1;
    unsigned int k:1;
    unsigned int l:1;
    unsigned int m:1;
    unsigned int n:1;
    unsigned int o:1;
    unsigned int p:1;
    unsigned char aaa;
    unsigned char bbb;
}bitint;
typedef union{
    bitint bit_data;
    uint data;
}Status;
class FormMockIO : public IDisplay
{
    Q_OBJECT

public:
    explicit FormMockIO(QWidget *parent = 0);
    ~FormMockIO();

    virtual void GetData(QByteArray &data);

public slots:
    virtual void beginDisplay();

    virtual QByteArray getBuffer();

private slots:
    void on_DI_LoopTrig2_clicked(bool checked);

    void on_DI_LoopFront_clicked(bool checked);

    void on_DI_LoopTrig1_clicked(bool checked);

    void on_DI_LoopBack_clicked(bool checked);

    void on_DI_BarrierDown_clicked(bool checked);

    void on_DI_BarrierUp_clicked(bool checked);
    //由于输出未使用看情况更改
        void on_DI_AlarmDetector_clicked(bool checked);
        void on_DO_CanopyLightGreen_clicked(bool checked);

        void on_DO_CanopyLightRed_clicked(bool checked);

        void on_DO_PassLightGreen_clicked(bool checked);

        void on_DO_PassLightRed_clicked(bool checked);

        void on_DO_BarrierDown_clicked(bool checked);

        void on_DO_BarrierUp_clicked(bool checked);

        void on_DO_AlarmLight_clicked(bool checked);

        void on_DO_AlarmSound_clicked(bool checked);
private:
    Ui::FormMockIO *ui;
    QByteArray m_io;
    Status instatus;
    Status outstatus;
};
#endif // FORMMOCKIO_H
