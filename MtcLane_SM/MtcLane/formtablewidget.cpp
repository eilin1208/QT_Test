#include "formtablewidget.h"
#include "ui_formtablewidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
const QString SelectQLabelStyle = "color:blue;";
const QString DisableQLabelStyle = "color:gray;";
const char* PageString = "↑ 上页 第%1页 共%2页 ↓ 下页";
const char* Note = "【数字】键选择 【确定】键确认 【ESC】键返回";
const char* NoteWithoutEsc = "【数字】键选择 【确定】键确认";
//每列最多显示5个项
const int MAXITEMLENGTH = 5;
FormTableWidget::FormTableWidget(QWidget *parent, QLayout *layout)
    :MtcOpWidget(parent,layout),
      ui(new Ui::FormTableWidget),m_nRow(0),m_nCol(1),m_nCurrentIndex(0),m_nLastIndex(0),
      m_nPage(0), m_nLastPage(0),m_bEscEnable(false), m_nLength(0)
{
    ui->setupUi(this);
    //ui->containerTitle->setFont();
}

FormTableWidget::~FormTableWidget()
{
    ////qDebug() << "release ....... " << objectName();
    delete ui;
}

void FormTableWidget::setCurrentIndex(int index)
{
    int realIndex = m_nPage * 10 + index;
    if(realIndex > (m_qItems.length() - 1))
    {
        return;
    }

    if(m_qItems.at(index)->isEnabled() == false)
    {
        return;
    }
    ////qDebug() << m_qItems.length();
    m_qItems[m_nCurrentIndex]->setStyleSheet("");
    m_nLastIndex = m_nCurrentIndex;
    m_nCurrentIndex = index + m_nPage * 10;
    m_qItems[m_nCurrentIndex]->setStyleSheet(SelectQLabelStyle);
    m_sCurrentValue = m_qItems[m_nCurrentIndex]->property("value");
    QString text = m_qItems[m_nCurrentIndex]->text();
    m_sCurrentText = text.right(text.length() - 2);
}

void FormTableWidget::mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent)
{
    if(mtcKeyEvent->isNumKey())
    {
        mtcKeyEvent->setKeyType(KC_Number);
        int selNum = mtcKeyEvent->getLogicKeyName().toInt();
        if(selNum - 1 == currentIndex() ||(selNum == 0 && currentIndex() == 9))
        {
            mtcKeyEvent->setKeyType(KC_NONE);
            return;
        }
        if(selNum > 0)
        {
            setCurrentIndex(selNum - 1);
        }
        if(selNum == 0)
        {
            setCurrentIndex(9);
        }
        return;
    }
    if(mtcKeyEvent->isFuncKey())
    {
        if(mtcKeyEvent->getLogicKey() == KeyUp)
        {
            //已经到达第一页，不进行任何操作
            if(m_nPage == 0)
            {
                return;
            }
            showBackPage();
        }
        if(mtcKeyEvent->getLogicKey() == KeyDown)
        {
            //已经到达最后一页，不进行任何操作
            if(m_nPage == m_nLastPage)
            {
                return;
            }
            showNextPage();

        }
        //如果是domodalshow则需要处理enter按键和esc按键
        if(m_pEventLoop != 0)
        {
            if(mtcKeyEvent->func() == KeyConfirm)
            {
                setResult(currentIndex());
            }
            else if(mtcKeyEvent->func() == KeyEsc)
            {
                setResult(-1);
            }
            mtcKeyEvent->setKeyType(KC_Func);
        }
    }
}

int FormTableWidget::row()
{
    return ui->gridLayout->rowCount();
    this->setProperty("row",2);
    this->property("row").toInt();
}

bool FormTableWidget::currentEnable()
{
    return m_qItems[currentIndex()]->isEnabled();
}

void FormTableWidget::showBackPage()
{
    clearPage();
    m_nPage--;
    showPage(m_nPage);
    ui->label->setText(tr(PageString).arg(m_nPage + 1).arg(m_nLastPage + 1));
    setCurrentIndex(0);
}

void FormTableWidget::showNextPage()
{
    clearPage();
    m_nPage++;
    showPage(m_nPage);
    ui->label->setText(tr(PageString).arg(m_nPage + 1).arg(m_nLastPage + 1));
    setCurrentIndex(0);
}

void FormTableWidget::clearPage()
{
    foreach(QLabel* item, m_qItems)
    {
        item->setVisible(false);
    }
}

void FormTableWidget::moveItemtoLayout()
{
    //根据项的编号，除于5为偶数的放在左侧，为奇数的放在右侧
    int tempCol = 0;
    for(int i = 0; i < m_nLength; i++)
    {
        tempCol = (i / MAXITEMLENGTH) % m_nCol;
        ui->gridLayout->addWidget(m_qItems[i], (i % MAXITEMLENGTH), tempCol);
    }
}

void FormTableWidget::showPage(int page)
{
    clearPage();
    int startIndex = page * MAXITEMLENGTH * m_nCol;
    int count = MAXITEMLENGTH * m_nCol;
    if(page == m_nLastPage)
    {
        count = qMin(m_nLength - startIndex,MAXITEMLENGTH * m_nCol);
    }
    int index = 0;
    for(int i = startIndex; i < count + startIndex; i++,index++)
    {
        m_qItems.at(i)->setVisible(true);
    }
}

void FormTableWidget::setEscEnable(bool enable)
{
    m_bEscEnable = enable;
    if(enable)
    {
        ui->label_2->setText(Note);
    }
    else
    {
        ui->label_2->setText(NoteWithoutEsc);
    }
}

void FormTableWidget::insertItem(const QString &itemText, const QVariant& itemValue, bool itemEnable)
{
    QLabel* tempLabel = new QLabel;
    tempLabel->setText(tr("%1.%2").arg((m_nLength + 1) % 10 ).arg(itemText));
    tempLabel->setProperty("value", itemValue);
    tempLabel->setEnabled(itemEnable);
    //tempLabel->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    if(!itemEnable)
    {
        tempLabel->setStyleSheet(DisableQLabelStyle);
    }

    //ui->gridLayout->addWidget(tempLabel, m_nLength / m_nCol, m_nLength % m_nCol);
    m_qItems.append(tempLabel);
    m_nLength++;
}

void FormTableWidget::showEvent(QShowEvent *event)
{

    moveItemtoLayout();
        //显示
        showPage(m_nPage);
        //如果项数多于一页，显示分页提示
        if(m_nLength > MAXITEMLENGTH * m_nCol)
        {
            if(m_bEscEnable)
            {
                ui->label_2->setText(Note);
            }
            else
            {
                ui->label_2->setText(NoteWithoutEsc);
            }
            m_nLastPage = m_nLength / MAXITEMLENGTH / m_nCol;
            ui->label->setText(tr(PageString).arg(m_nPage + 1).arg(m_nLastPage + 1));
        }
        else
        {
            ui->label_2->setText("请选择");
            ui->label->setText("");
        }

        //设置默认选择
        if(row() > 0)
        {
            setCurrentIndex(0);
        }


}

//void FormTableWidget::hideEvent(QHideEvent *event)
//{
//    while(ui->gridLayout->count() != 0)
//    {
//        QWidget *tempWidget = ui->gridLayout->itemAt(0)->widget();
//        ui->gridLayout->removeWidget(tempWidget);
//        tempWidget->deleteLater();
//    }
//}
void FormTableWidget::hideEvent(QHideEvent *event)
{

}

void FormTableWidget::clearData()
{
    //    m_nRow = 0;
    //    m_nCol = 1;
    //    m_nPage = 0;
    //    m_nLastPage = 0;
    //    m_nLength = 0;
    //    m_nCurrentIndex = 0;
    //    m_qItems.clear();
    //    m_sCurrentValue.clear();
    //    ui->containerTitle->clear();
    while(ui->gridLayout->count() != 0)
    {
        QWidget *tempWidget = ui->gridLayout->itemAt(0)->widget();
        ui->gridLayout->removeWidget(tempWidget);
        delete tempWidget;
    }
    m_nRow = 0;
    m_nCol = 1;
    m_nPage = 0;
    m_nLastPage = 0;
    m_nLength = 0;
    m_nCurrentIndex = 0;
    m_qItems.clear();
    m_sCurrentValue.clear();
    ui->containerTitle->clear();
}

FormTableWidget* getTableWidget(QWidget *widget, QLayout *layout)
{
    static FormTableWidget* g_pTableWidget = NULL;
    if(g_pTableWidget == NULL)
    {
        g_pTableWidget = new FormTableWidget(widget, layout);
    }
    return g_pTableWidget;
}
