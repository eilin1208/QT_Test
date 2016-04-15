#ifndef FORMTABLEWIDGET_H
#define FORMTABLEWIDGET_H
/*
NOTE:可复用选择Widget使用说明
使用该widget最好不要直接访问其函数，尽量使用setProperty接口设置属性，使用property接口获取属性，
对该widget设置选择项时，请注意以下几点：
1.必须先设置col属性再设置数据，否则会使用col默认值，即界面显示一列
2.当用户按下了esc键，同事widget是启用esc键响应的时候，widget会发送esc()信号

以下为属性说明
[col]
类型：int
属性名称："col"
说明：列数，此属性为widget显示数据时的列数

[currentIndex]
类型:int
属性名称："currentIndex"
说明:当前索引号，指用户选中的item的编号

[data]
类型:QStringList
属性名称："data"
说明:widget要显示的数据

[dataDisable]
类型：int
属性名称:"dataDisable"
说明：不可选中的item的编号

[currentText]
类型：QString
属性名称:"currentText"
说明：当前选中item的内容

[escEnable]
类型：bool
属性名称:"escEnable"
说明：是否响应esc按键
*/
#include <QWidget>
#include <MtcKey/MtcOpWidget.h>
#include <QLayout>
#include <QList>
#include <QLabel>
#include <QStringList>
namespace Ui {
class FormTableWidget;
}

class FormTableWidget : public MtcOpWidget
{
    Q_OBJECT
    Q_PROPERTY(int row READ row WRITE setRow )
    Q_PROPERTY(int col READ col WRITE setCol)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(QString currentText READ currentText)
    Q_PROPERTY(bool escEnable READ escEnable WRITE setEscEnable)
    Q_PROPERTY(int length READ length)
    Q_PROPERTY(QVariant currentValue READ currentValue)

public:
    explicit FormTableWidget(QWidget *parent=0, QLayout *layout=0);
    virtual ~FormTableWidget();

    enum emLayoutType
    {
        Vertical,           //纵向
        Horizontal,         //横向
        Table               //列表
    };
signals:
    void esc();
//属性
public:
    int row();
    QString currentText()
    {
        return m_sCurrentText;
    }
    bool currentEnable();
    int col()
    {
        return m_nCol;
    }
    void setRow(int row)
    {
        m_nRow = row;
    }
    void setCol(int col)
    {
        if(col > 0)
        {
            m_nCol = col;
        }
        else
        {
            m_nCol = 1;
        }
    }
    int currentIndex()
    {
        return m_nCurrentIndex;
    }
    void setCurrentIndex(int index);
    bool escEnable(){return m_bEscEnable;}
    void setEscEnable(bool enable);
    int length(){return m_nLength;}
    QVariant currentValue(){return m_sCurrentValue;}
//公共接口
public:
    //清除所有数据
    void clearData();
    //设置数据
    void insertItem(const QString& itemText, const QVariant& itemValue = 0, bool itemEnable = true);
protected:
    void mtcKeyPressed(MtcKeyPressedEvent *mtcKeyEvent);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
private:
    //显示上一页
    void showBackPage();
    //显示下一页
    void showNextPage();
    //显示页
    void showPage(int page);
    //清除页面上的所有数据
    void clearPage();
    //将所有项添加入layout
    void moveItemtoLayout();

private:
    Ui::FormTableWidget *ui;
    int m_nRow;             //行
    int m_nCol;             //列
    emLayoutType m_emLayoutType;        //布局类型
    QList<QLabel*> m_qItems;             //表项
    int m_nCurrentIndex;
    int m_nLastIndex;
    int m_nPage;
    int m_nLastPage;
    bool m_bEscEnable;
    int m_nLength;
    QVariant m_sCurrentValue;
    QString m_sCurrentText;
};

FormTableWidget *getTableWidget(QWidget* widget = NULL, QLayout* layout = NULL);

#endif // FORMTABLEWIDGET_H
