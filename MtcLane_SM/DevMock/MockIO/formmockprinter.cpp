#include "formmockprinter.h"
#include "ui_formmockprinter.h"
#include "datadefine.h"
QFile g_logPrt("./Printer.log");
//将信息写入log文件
void LogPrt(const QString& sLog)
{
    if(!g_logPrt.isOpen())
    {
        g_logPrt.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QString sFinal = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(sLog);
    g_logPrt.write(sFinal.toLocal8Bit());
        g_logPrt.flush();
}

FormMockPrinter::FormMockPrinter(QWidget *parent) :
    IDisplay(parent),
    ui(new Ui::FormMockPrinter)
{
    ui->setupUi(this);
}

FormMockPrinter::~FormMockPrinter()
{
    delete ui;
}

void FormMockPrinter::beginDisplay()
{
    this->setFocus();
    this->show();
    ui->plainTextEdit->clear();
    QString string;
    string = QString(m_prt);
    LogPrt(string);
    ui->plainTextEdit->appendPlainText(string);
}

QByteArray FormMockPrinter::getBuffer()
{
}

void FormMockPrinter::GetData(QByteArray &data)
{
    m_prt = data;
}
