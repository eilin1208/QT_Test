#include "formmcokvpr.h"
#include "ui_formmcokvpr.h"
#include "datadefine.h"
#include <QPixmap>
#include <QImage>
#include <QPalette>
#include <QFileDialog>
#include <QBuffer>
QByteArray platearr;
char* plate;
void random()
{
    char* listch[10];
    listch[0] = "赣G01010";
    listch[1] = "云Y23232";
    listch[2] = "鲁AC292S";
    listch[3] = "晋M12345";
    listch[4] = "沪U23232";
    listch[5] = "云A05739";
    listch[6] = "云B06331";
    listch[7] = "云F31139";
    listch[8] = "云T71531";
    listch[9] = "云E21639";
    int randn = qrand()%10;
    plate = listch[randn];
}
void Ten2Char(int num,char data[4])
{
    static const char* tab = "0123456789";
    data[0] = tab[num/1000];
    data[1] = tab[(num%1000)/100];
    data[2] = tab[((num%1000)%100)/10];
    data[3] = tab[((num%1000)%100)%10];
}
FormMcokVpr::FormMcokVpr(QWidget *parent) :
    IDisplay(parent),
    ui(new Ui::FormMcokVpr)
{
    ui->setupUi(this);
}
FormMcokVpr::~FormMcokVpr()
{
    delete ui;
}
void FormMcokVpr::on_comboBox_activated(int index)
{
    if(index == 0)
    {
        nColor = 0;
    }else if(index == 1)
    {
        nColor = VP_COLOR_WHITE;
    }else if(index == 2)
    {
        nColor = VP_COLOR_YELLOW;
    }else if(index == 3)
    {
        nColor = VP_COLOR_BLACK;
    }else
    {
        nColor = -1;
    }

}

void FormMcokVpr::on_push_open_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"), "./MockBmp/", tr("Image Files(*.jpg *.png)"));
    QPixmap pixmap;
    pixmap.load(filename);
    ui->picture->setPixmap(pixmap);
}

void FormMcokVpr::on_push_pic_clicked()
{
    m_vpr.clear();
    int charnum = 0;
    char datac[4];
    QString string;
    string  = ui->lineEdit->text();
    QByteArray temp,datemp;
    temp = string.toLocal8Bit();
    QBuffer buffer(&platearr);
    buffer.open(QIODevice::WriteOnly);
    ui->picture->pixmap()->save(&buffer,"JPG");
    buffer.close();
    //添加颜色到QByteArray
    datemp = QByteArray::number(nColor);
    qDebug()<<datemp.size()<<"  nColor";
    m_vpr.append(datemp);
    datemp.clear();
    //添加车牌字符串大小到QByteArray
    charnum = temp.size();
    Ten2Char(charnum,datac);
    m_vpr.append(datac[0]);
    m_vpr.append(datac[1]);
    m_vpr.append(datac[2]);
    m_vpr.append(datac[3]);
    qDebug()<<charnum<<"  ncharnum";
    //将车牌字符串添加到QByteArray
    m_vpr.append(temp);
    temp.clear();
    //将图像大小添加到QByteArray
    nBigImgSize = platearr.size();
    memset(datac,0,4);
    Ten2Char(nBigImgSize,datac);
    m_vpr.append(datac[0]);
    m_vpr.append(datac[1]);
    m_vpr.append(datac[2]);
    m_vpr.append(datac[3]);
    //将图像信息添加到QByteArray
    m_vpr.append(platearr);
    emit readySend();
}
void FormMcokVpr::on_push_rand_clicked()
{
    random();
    ui->lineEdit->setText(QString(plate));
}

void FormMcokVpr::beginDisplay()
{
    this->setFocus();
    this->show();
}

QByteArray FormMcokVpr::getBuffer()
{
    return m_vpr;
}

void FormMcokVpr::GetData(QByteArray &data)
{
}
