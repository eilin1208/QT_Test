#include "formcfgfile.h"
#include "ui_formcfgfile.h"
FormCfgFile::FormCfgFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCfgFile)
{
    ui->setupUi(this);
    ui->treeWidget_cfg->setHeaderLabels(m_MaintainCfg.setColumnName("收费软件配置文件"));
    m_MaintainCfg.getCfgInfo(ui->treeWidget_cfg);
    ui->treeWidget_cfg->addTopLevelItems(m_MaintainCfg.getTopNode());

    //设置显示属性
    ui->lineEdit_cfgItemName->setDisabled(true);
    ui->lineEdit_cfgItemCurrentVelue->setDisabled(true);
    ui->lineEdit_cfgItemDefaultValue->setDisabled(true);

    //信号槽连接
    connect(ui->treeWidget_cfg,SIGNAL(itemClicked(QTreeWidgetItem*,int))
            ,this,SLOT(clickItem(QTreeWidgetItem*,int)));
    connect(ui->pushButton_SaveCfg,SIGNAL(clicked())
            ,this,SLOT(clickSavePushbutton()));
    connect(ui->pushButton_ReCover,SIGNAL(clicked())
            ,this,SLOT(clickRecoverPushbutton()));
}

FormCfgFile::~FormCfgFile()
{
    delete ui;
}

void FormCfgFile::clickItem(QTreeWidgetItem *item, int column)
{
    QList<QTreeWidgetItem *>::iterator it;
    int index=0;
    for(it=m_MaintainCfg.listCfgData.begin();it!=m_MaintainCfg.listCfgData.end();++it,++index)
    {
        if((*it)==item)
            break;
    }
    if(index==m_MaintainCfg.listCfgData.size())
        return;
    quint16 iter;
    for(iter=0;iter!=m_MaintainCfg.listCfgNode.size();++iter)
    {
        if((m_MaintainCfg.listCfgNode.at(iter)->code==index))
        {
            ui->lineEdit_cfgItemName->setText((m_MaintainCfg.listCfgNode.at(iter)->cfg_name));
            ui->lineEdit_cfgItemDefaultValue->setText((m_MaintainCfg.listCfgNode.at(iter)->cfg_defaultValue));
            ui->lineEdit_cfgItemCurrentVelue->setText((m_MaintainCfg.listCfgNode.at(iter)->cfg_currentValue));
            ui->textEdit_introduce->setText((m_MaintainCfg.listCfgNode.at(iter)->cfg_introduce));
            m_code=iter;
            ui->lineEdit_cfgModify->clear();
            break;
        }
    }
    if(iter==m_MaintainCfg.listCfgNode.size())
    {
        ui->lineEdit_cfgItemName->clear();
        ui->lineEdit_cfgItemDefaultValue->clear();
        ui->lineEdit_cfgItemCurrentVelue->clear();
        ui->lineEdit_cfgModify->clear();
        ui->textEdit_introduce->clear();
    }

}

void FormCfgFile::clickSavePushbutton()
{
    if(ui->lineEdit_cfgModify->text().isEmpty())
    {
        ui->textEdit_introduce->clear();
        ui->textEdit_introduce->append(tr("请输入你想要修改的值"));
        return;
    }
    //保存修改值
    m_MaintainCfg.listCfgNode.at(m_code)->cfg_ModifyValue=ui->lineEdit_cfgModify->text();
    MaintainCFG::WriteCfg(m_MaintainCfg.listCfgNode.at(m_code)->cfg_Path,
                          m_MaintainCfg.listCfgNode.at(m_code)->cfg_ModifyValue);
    ui->textEdit_introduce->clear();
    ui->textEdit_introduce->append(tr("当前修改保存成功"));
    //刷新当前值
    m_MaintainCfg.listCfgNode.at(m_code)->cfg_currentValue=ui->lineEdit_cfgModify->text();
}

void FormCfgFile::clickRecoverPushbutton()
{
    m_MaintainCfg.listCfgNode.at(m_code)->cfg_ModifyValue.clear();
    m_MaintainCfg.listCfgNode.at(m_code)->cfg_currentValue=m_MaintainCfg.mapHistory[m_MaintainCfg.listCfgNode.at(m_code)->code];
    MaintainCFG::WriteCfg(m_MaintainCfg.listCfgNode.at(m_code)->cfg_Path,m_MaintainCfg.mapHistory[m_MaintainCfg.listCfgNode.at(m_code)->code]);
    ui->textEdit_introduce->clear();
    ui->textEdit_introduce->append(tr("该项配置已恢复到修改前的值"));
}
