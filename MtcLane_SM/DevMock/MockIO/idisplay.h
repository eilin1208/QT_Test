#ifndef IDISPLAY_H
#define IDISPLAY_H
#include "datadefine.h"
#include <QWidget>

class IDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit IDisplay(QWidget *parent = 0);
   //当显示窗口时从MsgReceiver中获取数据
    virtual void GetData(QByteArray &data) = 0;
signals:
    //当要发送数据时，先发信号
    void readySend();

public slots:
    //开始显示各个设备窗口
    virtual void beginDisplay() = 0;
    //当发送数据时，msgReceiver会调用这个函数获取数据
    virtual QByteArray getBuffer() = 0;
};
//从QMap中获取需要的子类
IDisplay* getDisplay(int type);
//将各个设备的子函数加入QMap中
void addDisplay(int type, IDisplay* display);

#endif // IDISPLAY_H
