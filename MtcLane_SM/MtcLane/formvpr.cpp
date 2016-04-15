#include "formvpr.h"
#include "ui_formvpr.h"
#include <QDebug>
const int VPREVENTTYPE = 2005;
FormVpr::FormVpr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormVpr)
{
    ui->setupUi(this);
    ////qDebug() << "设置了对象" << VPREVENTTYPE;
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //去掉自动序号
    QHeaderView *headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);

}

FormVpr::~FormVpr()
{
    delete ui;
}

void FormVpr::setCapImage(const QString &sFile)
{
    QPixmap model(sFile);
    model = model.scaled(ui->label->width(), ui->label->height());
    ui->label->setPixmap(model);
}

void FormVpr::clearImage()
{
    ui->label->clear();
    ui->labVpr->clear();
    ui->label->setText(tr("车道摄像机\n抓拍图像"));
    ui->labVpr->setText(tr("车牌识别\n图像"));
}

void FormVpr::changePage()
{
    ui->stackedWidget->setCurrentIndex(!ui->stackedWidget->currentIndex());
}

void FormVpr::setLogger(QFileLogger *logger)
{
    if(logger != NULL)
    {
        connect(logger, SIGNAL(showLog(QString)), this, SLOT(insertLog(QString)));
    }
}

void FormVpr::insertLog(const QString &sLog)
{
    //判断行数是否已经满100条，如果满100条，删除最上一条
    if(ui->tableWidget->rowCount() >= 100)
    {
        ui->tableWidget->removeRow(0);
    }
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    int nRow = ui->tableWidget->rowCount() - 1;
    ui->tableWidget->setItem(nRow, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss")));
    ui->tableWidget->setItem(nRow, 1, new QTableWidgetItem(sLog));
}

void FormVpr::customEvent(QEvent *event)
{

    VPREVENT* pEvent = static_cast<VPREVENT*>(event);
    if(pEvent != NULL)
    {
        ////qDebug() << "获得了车牌识别结果" << pEvent->m_nDataMsgId << pEvent->m_nIndex;
        if(pEvent->m_nDataMsgId == VPREVENTTYPE)
        {
            int nIndex = pEvent->m_nIndex;
            bool bRet = getDeviceFactory()->getVpr()->SaveVPRResult(nIndex);
            if(bRet)
            {
                const CVPRResult* pVpr = getDeviceFactory()->getVpr()->GetCurrentResult();
                if(pVpr->HasBigImg())
                {
                    SetVehPlatePic(pVpr);
                }
                ////qDebug() << pVpr->HasBigImg() << pVpr->HasSmallImg();
                ////qDebug() << getDeviceFactory()->getVpr()->GetCurrentResult()->sVehPlate;
                emit onGetVpr(nIndex);
            }
        }
    }
}

bool FormVpr::SetVehPlatePic(const CVPRResult *pRlt)
{
    if(!pRlt)
    {
        ui->labVpr->clear();
        return false;
    }
    QPixmap model;
    model.loadFromData(pRlt->lpBigImgBuf, pRlt->nBigImgSize);
    model = model.scaled(ui->labVpr->width(),ui->labVpr->height());
    ui->labVpr->setPixmap(model);
    return true;
}

void FormVpr::initPage()
{
    ui->stackedWidget->setCurrentIndex(0);
}
