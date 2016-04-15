#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "BaseInterface/devbaseinterface.h"
#include "PrinterArm/printerarm.h"
namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    //填充PrinterArm的m_NoteContent
    void SetNoteContent();

private slots:
    void on_pushButton_openDev_clicked();

    void on_pushButton_CloseDev_clicked();

    void on_pushButton_printer_clicked();

private:
    Ui::Widget *ui;
    DevBaseInterface* tmpDev;
    PrinterArm* printer;
};

#endif // WIDGET_H
