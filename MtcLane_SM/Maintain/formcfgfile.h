#ifndef FORMCFGFILE_H
#define FORMCFGFILE_H

#include <QWidget>
#include "maintaincfg.h"
namespace Ui {
    class FormCfgFile;
}

class FormCfgFile : public QWidget
{
    Q_OBJECT

public:
    explicit FormCfgFile(QWidget *parent = 0);
    ~FormCfgFile();

public slots:
    //点击某一项后处理
    void clickItem(QTreeWidgetItem * item, int column );
    //点击保存按钮
    void clickSavePushbutton();
    //点击回复配置按钮
    void clickRecoverPushbutton();
private:
    Ui::FormCfgFile *ui;
    //参数文件配置
    MaintainCFG m_MaintainCfg;
    //当前选择的item的编号
    quint8 m_code;
    //当前历史记录
    QString s_history;
};

#endif // FORMCFGFILE_H
