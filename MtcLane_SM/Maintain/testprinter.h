#ifndef TESTPRINTER_H
#define TESTPRINTER_H

#include <QWidget>
#include "BaseInterface/devbaseinterface.h"
#include "PrinterArm/printerarm.h"
namespace Ui {
    class TestPrinter;
}

class TestPrinter : public QWidget
{
    Q_OBJECT

public:
    explicit TestPrinter(QWidget *parent = 0);
    ~TestPrinter();

    //填充PrinterArm的m_NoteContent
    void SetNoteContent();

private slots:
    void on_pushButton_openDev_clicked();

    void on_pushButton_CloseDev_clicked();

    void on_pushButton_printer_clicked();

private:
    Ui::TestPrinter *ui;
    DevBaseInterface* tmpDev;
    PrinterArm* printer;
};

#endif // TESTPRINTER_H
