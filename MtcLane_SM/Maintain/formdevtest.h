#ifndef FORMDEVTEST_H
#define FORMDEVTEST_H
#include <QLabel>
#include <QWidget>
#include <QMap>
enum TestDevice{
    DEV_Info,
    DEV_CardReader,            //读写器
    DEV_IOCard,                //IO设备
    DEV_SmallPrinter,          //票打
    DEV_CardManageMachine,     //卡机
    DEV_VideoCard,             //视频采集卡
    DEV_VehPlateRecognizer,    //车牌识别
    DEV_FareDisplayer,         //费显
    DEV_VDM,                   //字符叠加
    DEV_WEIGHT                 //记重设备
};

namespace Ui {
    class FormDevTest;
}

class FormDevTest : public QWidget
{
    Q_OBJECT

public:
    explicit FormDevTest(QWidget *parent = 0);
    ~FormDevTest();

    void Init();

private slots:
    void on_pushButton_fdTest_clicked();

    void on_pushButton_ioTest_clicked();

    void on_pushButton_VprTest_clicked();

    void on_pushButton_VedioCardTest_clicked();

    void on_pushButton_PrinterTest_clicked();

    void on_pushButton_WtTest_clicked();
    void on_pushButton_cardreader_clicked();

private:
    void createPages();

    int addPages(QWidget* page);
private:
    Ui::FormDevTest *ui;
    QMap<TestDevice, int> m_DevIndex;
    QLabel *devinfo;
};

#endif // FORMDEVTEST_H
