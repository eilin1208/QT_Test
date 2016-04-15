#include "formloadparam.h"
#include "ui_formloadparam.h"
#include "ParamModule/qparammgr.h"
#include "formmainwindow.h"
FormLoadParam::FormLoadParam(QWidget *parent, QLayout *layout) :
    MtcOpWidget(parent, layout),
    ui(new Ui::FormLoadParam)
{
    ui->setupUi(this);
    connect(getParamMgr(), SIGNAL(FileLoadSuccess(int)), this, SLOT(ShowFileInfo(int)), Qt::QueuedConnection);
    connect(getReqParam(), SIGNAL(ShowDownloadingFile(quint32)),
            this, SLOT(ShowDownloading(quint32)));
    connect(getReqParam(), SIGNAL(ShowPreDownloadResult(quint32,bool,bool)),
            this, SLOT(ShowPreDownload(quint32,bool,bool)));
    connect(getReqParam(), SIGNAL(ShowDownloadedResult(quint32,bool,bool)),
            this, SLOT(ShowDownloaded(quint32,bool,bool)));
    connect(getReqParam(), SIGNAL(LoadFinish(int, bool)),
            this, SLOT(ShowFileInfo(int)));

    //TODO_liubo
    /*ui->tableWidget->horizontalHeader()->resizeSection(0,144);
    ui->tableWidget->horizontalHeader()->resizeSection(1,105);
    ui->tableWidget->horizontalHeader()->resizeSection(2,96);
    ui->tableWidget->horizontalHeader()->resizeSection(3,144);*/
    //ui->tableWidget->horizontalHeader()->resizeSection(4,78);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QHeaderView *headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);

}

FormLoadParam::~FormLoadParam()
{
    ////qDebug() << "release ....... " << objectName();
    delete ui;
}


void FormLoadParam::ShowFileInfo(int nFile)
{
    qDebug() << "get here show!!!";
    int nRow = m_map.value(nFile);
    if(nRow==8)
            ui->tableWidget->scrollToBottom();

    QBaseParamFile* pFile = getParamMgr()->GetParamFile((CCfgFile)nFile);
    ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(pFile->GetVersionCode()));
    if(pFile->GetHasNewVersion())
    {
        ui->tableWidget->setItem(nRow, 2, new QTableWidgetItem(pFile->GetVersion(true).toString("yyyyMMddhhmmss")));
        ui->tableWidget->setItem(nRow, 3, new QTableWidgetItem(pFile->GetUseTime().toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void FormLoadParam::ShowDownloading(quint32 cfgFile)
{
}

void FormLoadParam::ShowPreDownload(quint32 cfgFile,bool bNew, bool bRet)
{
    QBaseParamFile* pFile = getParamMgr()->GetParamFile((CCfgFile)cfgFile);
    if(pFile)
    {
        QString sMsg = pFile->GetDispFileName() + tr("预下载") + (bRet? tr("成功") : tr("失败"));
        if(!bRet)
        {
            QString sMsgFile = pFile->GetDispFileName() + tr("   下载失败，三分钟后重新下载");

            ::showWarning(sMsgFile,true,FormWarning::WARNING);
        }
        LogMsg("lane",sMsg);
    }
}

void FormLoadParam::ShowDownloaded(quint32 cfgFile,bool bNew, bool bRet)
{
}

void FormLoadParam::ReLoadAll()
{
    getParamMgr()->LoadAllParamFile();
    connect(getParamMgr(), SIGNAL(AllFileLoadFinished()), getReqParam(), SLOT(StartAskParamFiles()));
    connect(getReqParam(), SIGNAL(ParamFilesLoadSuccess()), this, SLOT(quitModal()));
}

void FormLoadParam::showEvent(QShowEvent *event)
{
    //清除所有记录
    int nRowCount = ui->tableWidget->rowCount();
    for(int i = 0; i < nRowCount; i++)
    {
        ui->tableWidget->removeRow(0);
    }
    QBaseParamFile* pFile = NULL;
    int nCurRow = 0;
    //显示所有要加载的文件
    for(int i = 0 ; i < cfEnd; i++)
    {
        pFile = getParamMgr()->GetParamFile((CCfgFile)i);
        if(pFile)
        {
            ui->tableWidget->insertRow(nCurRow);
            ui->tableWidget->setItem(nCurRow, 0, new QTableWidgetItem(pFile->GetDispFileName()));
            m_map[i] = nCurRow;
            nCurRow++;
        }
    }
}

void FormLoadParam::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isFuncKey())
    {
        if(m_pEventLoop != 0)
        {
            switch(mtcKeyEvent->func())
            {
            case KeyConfirm:
                getReqParam()->Term();
                setResult(1);
                break;
            case KeyEsc:
                getReqParam()->Term();
                setResult(0);
                break;
            default:
                break;
            }
        }
    }
    mtcKeyEvent->setKeyType(KC_Func);
}

FormLoadParam* getLoadParam(QWidget *widget, QLayout *layout)
{
    static FormLoadParam* pLoadParam = NULL;
    if(pLoadParam == NULL)
    {
        pLoadParam = new FormLoadParam(widget, layout);
    }
    return pLoadParam;
}
