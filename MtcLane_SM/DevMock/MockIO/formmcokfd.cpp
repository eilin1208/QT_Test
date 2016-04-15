#include "formmcokfd.h"
#include "ui_formmcokfd.h"
QFile g_logFile("./FD.log");
//将信息写入log文件
void LogFD(const QString& sLog)
{
    if(!g_logFile.isOpen())
    {
        g_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QString sFinal = QObject::tr("[%1] %2\n")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(sLog);
    g_logFile.write(sFinal.toLocal8Bit());
    g_logFile.flush();
}
FormMcokFD::FormMcokFD(QWidget *parent) :
    IDisplay(parent),
    ui(new Ui::FormMcokFD)
{
    ui->setupUi(this);
}

FormMcokFD::~FormMcokFD()
{
    delete ui;
}

void FormMcokFD::beginDisplay()
{
    this->setFocus();
    this->show();
    ui->plainTextEdit->clear();
    QString string  = QString(m_fd);
    ui->plainTextEdit->appendPlainText(string);
    LogFD(string);
}

QByteArray FormMcokFD::getBuffer()
{
}

void FormMcokFD::GetData(QByteArray &data)
{
    m_fd = data;
}
