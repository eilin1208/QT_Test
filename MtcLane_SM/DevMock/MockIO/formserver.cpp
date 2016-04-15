#include "formserver.h"
#include "ui_formserver.h"
#include <QtNetwork/QHostInfo>
#include "formmcokfd.h"
#include "formmcokvpr.h"
#include "formmockio.h"
#include "formmockprinter.h"
#include "formmockweight.h"
#include <QtNetwork/QHostAddress>
FormServer::FormServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormServer)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any,19999);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnect()),Qt::DirectConnection);
    InitDsp();
}
FormServer::~FormServer()
{
    delete ui;
}
void FormServer::newConnect()
{
    if(tcpServer->hasPendingConnections())
    {
        MsgReceiver* receiver = new MsgReceiver;
        connect(receiver,SIGNAL(ReadyStr(int&,QString&)),this,SLOT(getStr(int&,QString&)),Qt::DirectConnection);
        receiver->setSocket(tcpServer->nextPendingConnection());        
        m_receivers.push_back(receiver);
    }
}

void FormServer::getStr(int & choice,QString &string)
{
    m_choice = choice;
    m_string = string;
    if(m_choice == 1)
    {
        ui->text_net->append(m_string);
    }
    if(m_choice == 2)
    {
        ui->text_io->append(m_string);
    }
    if(m_choice == 3)
    {
        ui->textEdit_prt->append(m_string);
    }
    if(m_choice == 4)
    {
        ui->textEdit_fd->append(m_string);
    }
    if(m_choice == 5)
    {
        ui->text_vpr->append(m_string);
    }
    if(m_choice == 6)
    {
        ui->textEdit_wt->append(m_string);
    }
    m_string.clear();
}
void FormServer::InitDsp()
{
    FormMcokFD* mockfd = new FormMcokFD();
    addDisplay(DEV_FD,mockfd);
    FormMcokVpr* mockVpr = new FormMcokVpr();
    addDisplay(DEV_VPR,mockVpr);
    FormMockIO* mockio = new FormMockIO();
    addDisplay(DEV_IO,mockio);
    FormMockPrinter* mockprt = new FormMockPrinter();
    addDisplay(DEV_PRT,mockprt);
    FormMockWeight* mockwt = new FormMockWeight();
    addDisplay(DEV_WT,mockwt);
}

