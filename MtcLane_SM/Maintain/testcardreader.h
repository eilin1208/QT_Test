#ifndef TESTCARDREADER_H
#define TESTCARDREADER_H

#include <QWidget>
#include "DevModule/BaseInterface/devbaseinterface.h"
#include "CardReaderArm/qcardreader.h"
namespace Ui {
    class TestCardReader;
}

class TestCardReader : public QWidget
{
    Q_OBJECT

public:
    explicit TestCardReader(QWidget *parent = 0);
    ~TestCardReader();

    void getCardTollInfo(TCardTollInfo CardTollInfo);

private slots:
    void on_pb_init_clicked();

    void on_pb_close_clicked();

    void on_pb_readcard_clicked();

    void on_writecard_clicked();
private:
    Ui::TestCardReader *ui;
    DevBaseInterface* tmpDev;
    QCardReader* reader;
    QString tmpcardid;
    QString m_str;
};

#endif // TESTCARDREADER_H
