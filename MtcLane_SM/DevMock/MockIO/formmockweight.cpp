#include "formmockweight.h"
#include "ui_formmockweight.h"
#include "datadefine.h"
int row ;
long numb[50];
int numb2;
int TypeNum[10]={0,1,1,2,2,2,3,3,3,3};//分别是0123456789型轴型轴组对应的轴数
FormMockWeight::FormMockWeight(QWidget *parent) :
    IDisplay(parent),
    ui(new Ui::FormMockWeight)
{
    ui->setupUi(this);
    row = 0;
}
FormMockWeight::~FormMockWeight()
{
    row = 0;
    delete ui;
}
/*数据发送时按照一定的规则，由于数据大部分为int或long型，可以加'>'或'<'等将各个数据分割
  在按照这个规矩解析。数据依次是：
1.carinfo.valid：//直接当作判断是单轴或轴组，1为轴组，不考虑轴组情况。
2.carinfo.GMflag://当作倒车的标志位，若为倒车，则设置为1；
3.carinfo.Speed
4.为单轴则为carinfo.AxleNum，接着是carinfo.AxleWt[i]；
*/
void FormMockWeight::on_push_send_clicked()
{
    if(row > 9)
    {
        ui->tableWidget->insertRow(row+1);
    }//new QTableWidgetItem
    ui->tableWidget->setItem(row,0,new QTableWidgetItem(QDateTime::currentDateTime().toString("dd:hh-mm-ss-zzz")));
    ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString::number(m_type)));
    numb2++;
    ui->label_allnumb->setText(QString("0000000")+QString::number(numb2));
    m_wt.clear();
    QByteArray temp;
    m_wt.insert(0,carinfo.valid);
    m_wt.insert(1,carinfo.GMflag);
    m_wt.append('>');//0
    temp = QByteArray::number(carinfo.Speed);
    ui->tableWidget->setItem(row,4,new QTableWidgetItem(QString::number(carinfo.Speed)));
    m_wt.append(temp);
    m_wt.append('>');//1
    temp.clear();
    temp = QByteArray::number(carinfo.AxleNum);
    ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString(temp)));
    m_wt.append(temp);
    temp.clear();
    m_wt.append('>');//2
    long m_allwt;
    for(int i = 0; i < carinfo.AxleNum; i++)
    {
        m_allwt = m_allwt + carinfo.AxleWt[i];
        ui->tableWidget->setItem(row,5+i,new QTableWidgetItem(QString::number(carinfo.AxleWt[i])));
        temp = QByteArray::number((int)carinfo.AxleWt[i]);
        m_wt.append(temp);
        temp.clear();
        m_wt.append('<');
    }
    ui->tableWidget->setItem(row,3,new QTableWidgetItem(QString::number(m_allwt)));
    emit readySend();
    row++;
}

void FormMockWeight::on_pushRand_clicked()
{
    carinfo.Init();
    carinfo.Speed = qrand()%20;
    carinfo.GMflag = '0';
    carinfo.valid = '1';
    QString string;
    int rspeed;
    rspeed = (qrand()%10)*10;
    string = QString::number(rspeed);
    ui->speed->setText(string);
    carinfo.Speed = rspeed;
    carinfo.AxleNum = qrand()%7+1;
    for(int i = 0; i < carinfo.AxleNum; i++)
    {
        carinfo.AxleWt[i] = (qrand()%15)*1000+1000;
        numb[i] = carinfo.AxleWt[i];
    }
    ui->line_axle->setText(QString::number(carinfo.AxleNum));
//    ui->line_group->setText(QString::number(carinfo.AxleGroupNum));

    //下面是对轴重量编辑框的分别赋值；
    if(numb[0] > 0)
    {
        ui->wt1->setText(QString::number(numb[0]));
    }
    else
    {
        ui->wt1->setText(QString::number(1000));
    }
    if(numb[1] > 0)
    {
        ui->wt2->setText(QString::number(numb[1]));
    }
    else
    {
        ui->wt2->setText(QString::number(1000));
    }
    if(numb[2] > 0)
    {
        ui->wt3->setText(QString::number(numb[2]));
    }
    else
    {
        ui->wt3->setText(QString::number(1000));
    }
    if(numb[3] > 0)
    {
        ui->wt4->setText(QString::number(numb[3]));
    }
    else
    {
        ui->wt4->setText(QString::number(1000));
    }
    if(numb[4] > 0)
    {
        ui->wt5->setText(QString::number(numb[4]));
    }
    else
    {
        ui->wt5->setText(QString::number(1000));
    }

    if(numb[5] > 0)
    {
        ui->wt6->setText(QString::number(numb[5]));
    }
    else
    {
        ui->wt6->setText(QString::number(1000));
    }
    if(numb[6] > 0)
    {
        ui->wt7->setText(QString::number(numb[6]));
    }
    else
    {
        ui->wt7->setText(QString::number(1000));
    }
    if(numb[7] > 0)
    {
        ui->wt8->setText(QString::number(numb[7]));
    }
    else
    {
        ui->wt8->setText(QString::number(1000));
    }
    if(numb[8] > 0)
    {
        ui->wt9->setText(QString::number(numb[8]));
    }
    else
    {
        ui->wt9->setText(QString::number(1000));
    }

    if(numb[9] > 0)
    {
        ui->wt10->setText(QString::number(numb[9]));
    }
    else
    {
        ui->wt10->setText(QString::number(1000));
    }
    m_type = qrand()%150;
    ui->line_type->setText(QString::number(m_type));
    for(int i = 0; i < 50; i++)
    {
        numb[i] = 0;
    }
}

void FormMockWeight::on_push_back_clicked()
{
    carinfo.GMflag = '1';
}

void FormMockWeight::on_push_clear_clicked()
{
    ui->tableWidget->clearContents();
    row = 0;

}
QByteArray FormMockWeight::getBuffer()
{
    return m_wt;
}
void FormMockWeight::beginDisplay()
{
    this->show();
    this->setFocus();
}

void FormMockWeight::GetData(QByteArray &data)
{
}
