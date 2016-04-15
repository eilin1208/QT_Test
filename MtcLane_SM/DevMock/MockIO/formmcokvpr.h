#ifndef FORMMCOKVPR_H
#define FORMMCOKVPR_H
#include <QWidget>
#include "idecoder.h"
#include "idisplay.h"
namespace Ui {
    class FormMcokVpr;
}
enum VP_COLOR
{
    VP_COLOR_NONE=-1,
    VP_COLOR_BLUE=0,
    VP_COLOR_YELLOW,
    VP_COLOR_BLACK,
    VP_COLOR_WHITE
};
#pragma pack(push, 1)
typedef struct
{
        int nColor;					//	车牌颜色代码
        char* lpszPlate;			//	车牌号 - 至少分配20字节内存
        quint32 nRating;				//	车牌的可信度 - 0-100
        quint32 nBigImgSize;			//	车辆大图图像大小
        unsigned char* lpBigImgBuf;			//	车辆大图图像数据缓冲区
}CAutoDetectedVehInfo;
#pragma pack(pop)
class FormMcokVpr : public IDisplay
{
    Q_OBJECT

public:
    explicit FormMcokVpr(QWidget *parent = 0);
    ~FormMcokVpr();

    virtual void GetData(QByteArray &data);
public slots:

    virtual void beginDisplay();

    virtual QByteArray getBuffer();

private slots:
    void on_comboBox_activated(int index);

    void on_push_open_clicked();

    void on_push_pic_clicked();

    void on_push_rand_clicked();

private:
    Ui::FormMcokVpr *ui;
    QByteArray m_vpr;
    int nColor;
    int nBigImgSize;			//	车辆大图图像大小
};


#endif // FORMMCOKVPR_H
