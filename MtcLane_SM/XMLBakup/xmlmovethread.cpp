#include "xmlmovethread.h"
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include "CommonModule/laneutils_new.h"
#include "CommonModule/ilogmsg.h"
#include <QDateTime>
#include <QDate>
#include "laneinfo.h"
XMLMoveThread::XMLMoveThread(QObject *parent) :
    QThread(parent)
{
}


void XMLMoveThread::run()
{

    //先判断是否有目录,如果没有则创建
    QDir dir;
    QString dirName = tr("/SDHC/file/%1")
            .arg(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
    //先判断是否有SD卡,每次向SD卡移动数据时都需要判断
    if(!dir.exists("/SDHC"))
    {
        LogMsg("lane", tr("未检测到SD卡,退出备份"));
        ///=qDebug()<<tr("未检测到SD卡,退出xml备份");
        return;
    }
    if(!dir.exists(dirName))
    {
        if(!dir.mkpath(dirName))
        {
            LogMsg("lane", tr("备份线程[%1]文件夹创建失败").arg(dirName));
            return;
        }
    }
    //获取tmpBakUp目录下所有文件信息
    dir.setPath("./tmpBakUp");
    QFileInfoList fileList = dir.entryInfoList(QDir::Files, QDir::Time);
    //转移文件,保留最新创建的一个
    QFileInfo fileInfo;
    for(int i = fileList.count() - 1; i > 0 ; i--)
    {
        //qDebug() << fileList.count() << i;
        fileInfo = fileList.at(i);
        if(fileInfo.exists())
        {
            if(!dir.exists("/SDHC"))
            {
                LogMsg("lane", tr("未检测到SD卡,退出备份"));
                //qDebug()<<tr("未检测到SD卡,退出xml备份");
                return;
            }
            if(!QFile::copy(fileInfo.filePath(), dirName + "/" + fileInfo.fileName()))
            {
                LogMsg("lane", tr("%1文件复制失败，退出此次备份").arg(fileInfo.filePath()));
                break;
            }
            if(!QFile::remove(fileInfo.filePath()))
            {
                LogMsg("lane", tr("%1文件删除失败，退出此次备份").arg(fileInfo.filePath()));
                break;
            }
            //qDebug() << fileList.count() << i <<fileInfo.fileName() << "bakup success";
            msleep(500);
        }
    }

    //转移图片文件 liujian
    movePicFile();
    //sd卡内数据删除
    DelSDPicFile();
    DelSDXmlFile();
}
void XMLMoveThread::movePicFile()
{
    //加载图片文件夹
    QDir dir;
    if(!dir.exists("/SDHC"))
    {
        LogMsg("lane", tr("未检测到SD卡,退出备份"));
        //qDebug()<<tr("未检测到SD卡,退出图片备份");
        return;
    }
    dir.setPath("/Program_Data");
    QFileInfoList fileList = dir.entryInfoList(QDir::Dirs,QDir::Time);
    QFileInfo fileInfo;
    int fileCount = fileList.count();
    for(int i = fileCount - 1 ; i >= 0; i--)
    {
        //        if(fileCount <= 3)
        //        {
        //            qDebug()<<tr("保留当天文件夹");
        //            return;
        //        }
        //qDebug()<<tr("检测到图片文件夹数量：%1名称：%2").arg(fileList.count()).arg(fileList.at(i).fileName());
        fileInfo = fileList.at(i);
        //每个图片文件夹中的文件(去除根目录与最新文件夹)
        if(fileInfo.exists() && !fileInfo.fileName().startsWith(".") && fileInfo.created().date() != QDate::currentDate())
        {
            //sd卡中创建同名图片文件夹
            QDir picDir;
            QString SDPicDirName = "/SDHC/PicFile/"+fileInfo.fileName();
            //qDebug()<<tr("检测到图片文件夹%1").arg(SDPicDirName);
            if(!picDir.exists(SDPicDirName))
            {
                if(!picDir.mkpath(SDPicDirName))
                {
                    LogMsg("lane", tr("备份线程:图片文件夹%1创建失败").arg(SDPicDirName));
                    return;
                }
            }
            picDir.setPath("/Program_Data/"+fileInfo.fileName());
            QFileInfoList picList = picDir.entryInfoList(QDir::Files,QDir::Time);
            QFileInfo picFileInfo;
            bool delOver = true;
            for(int i = 0;i < picList.count();i++)
            {
                picFileInfo = picList.at(i);
                if(picFileInfo.exists())
                {
                    //qDebug()<<tr("图片文件：%1").arg(picFileInfo.fileName());
                    if(!dir.exists("/SDHC"))
                    {
                        LogMsg("lane", tr("未检测到SD卡,退出备份"));
                        //qDebug()<<tr("未检测到SD卡,退出图片备份");
                        return;
                    }
                    if(!QFile::copy(picFileInfo.filePath(),SDPicDirName+"/" + picFileInfo.fileName()))
                    {
                        LogMsg("lane", tr("%1文件复制失败，退出此次备份").arg(picFileInfo.filePath()));
                        //qDebug()<<tr("%1文件复制失败，退出此次备份").arg(picFileInfo.absoluteFilePath());
                        delOver = false;
                        break;
                    }
                    if(!QFile::remove(picFileInfo.filePath()))
                    {
                        LogMsg("lane", tr("%1文件删除失败，退出此次备份").arg(picFileInfo.filePath()));
                        delOver = false;
                        break;
                    }
                }
                msleep(500);
            }
            //文件夹内文件全部转移后删除文件夹
            if(delOver == true)
            {
                dir.rmdir(fileInfo.filePath());
                //fileCount = fileCount-1;
            }
        }
    }
}
void XMLMoveThread::DelSDPicFile()
{
    //qDebug()<<tr("删除图片开始");
    QDir dir;
    if(!dir.exists("/SDHC"))
    {
        LogMsg("lane", tr("未检测到SD卡,不执行SD卡文件清理操作"));
        //qDebug()<<tr("未检测到SD卡,不执行SD卡文件清理操作");
        return;
    }
    QString dirName = "/SDHC/PicFile";
    if(!dir.exists(dirName))
    {
        LogMsg("lane", tr("SD中不存在图片备份,不执行SD卡文件清理操作"));
        //qDebug()<<tr("SD中不存在图片备份,不执行SD卡文件清理操作");
        return;
    }
    QDate curDate = QDateTime::currentDateTime().addDays(-getLaneInfo()->getPicSaveDay()).date();
    QString tmpFileName = curDate.toString("MMdd");
    dir.setPath(dirName);
    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs,QDir::Time);
    QFileInfo dirInfo;
    int count = dirList.count();
    for(int i = count - 1;i > 0;i--)
    {
        //qDebug()<<tr("SD卡图片文件夹数量：%1").arg(count);
        //保留一个文件夹
        //        if(count <= 3)
        //        {
        //            qDebug()<<tr("保留一个文件夹");
        //            return;
        //        }
        dirInfo = dirList.at(i);
        if(!dirInfo.fileName().startsWith(".") && dirInfo.created().date() <= curDate)
            //if(dirInfo.fileName() == tmpFileName)
        {
            tmpFileName = dirInfo.fileName();
            //qDebug()<<tr("确定要删除文件目录：%1").arg(tmpFileName);
            dir.setPath(dirName + "/" + tmpFileName);
            QFileInfoList fileList = dir.entryInfoList(QDir::Files,QDir::Time);
            for(int i = 0;i < fileList.count();i++)
            {
                if(!dir.exists("/SDHC"))
                {
                    LogMsg("lane", tr("文件清理过程中检测不到SD卡"));
                    //qDebug()<<tr("文件清理过程中检测不到SD卡");
                    return;
                }
                QFile::remove(fileList.at(i).filePath());
            }
            dir.rmdir(dirInfo.filePath());
            //count --;
        }
    }
}

void XMLMoveThread::DelSDXmlFile()
{
    qDebug()<<tr("删除xml开始");
    QString command= tr("find /SDHC/file -type d ! -name 'file' -prune -mtime +%1 -exec rm -rf {} \\;")
            .arg(getLaneInfo()->getXmlSaveDay());
    int ret = 0;
#ifndef Q_OS_WIN
    ret = system(command.toLocal8Bit().data());
#endif
    qDebug() << tr("执行删除操作完成command[%1],返回%2").arg(command).arg(ret);
    //    QDir dir;
    //    if(!dir.exists("/SDHC"))
    //    {
    //        LogMsg("lane", tr("未检测到SD卡,不执行SD卡文件清理操作"));
    //        qDebug()<<tr("未检测到SD卡,不执行SD卡文件清理操作");
    //        return;
    //    }
    //    QString dirName = "/SDHC/file";
    //    if(!dir.exists(dirName))
    //    {
    //        LogMsg("lane", tr("SD中不存在xml备份,不执行SD卡文件清理操作"));
    //        qDebug()<<tr("SD中不存在xml备份,不执行SD卡文件清理操作");
    //        return;
    //    }
    //    QDate curDate = QDateTime::currentDateTime().addDays(-getLaneInfo()->getXmlSaveDay()).date();
    //    QString tmpYDirName;
    //    QString tmpMDirName;
    //    QString tmpDDirName;
    //    dir.setPath(dirName);
    //    //遍历年文件夹
    //    QFileInfoList YDirList = dir.entryInfoList(QDir::Dirs,QDir::Time);
    //    qDebug() << YDirList.count();
    //    for(int i = YDirList.count()-1;i >=0;i--)
    //    {
    //        QFileInfo YDirInfo = YDirList.at(i);
    //        if(!YDirInfo.fileName().startsWith(".") && YDirInfo.created().date() <= curDate)
    //        {
    //            qDebug()<<tr("符合条件年文件夹：%1").arg(YDirInfo.fileName());
    //            tmpYDirName = YDirInfo.fileName();
    //            dir.setPath(dirName + "/" + tmpYDirName);
    //            //遍历月文件夹
    //            QFileInfoList MDirList = dir.entryInfoList(QDir::Dirs,QDir::Time);
    //            int curMFileNum = MDirList.count();
    //            for(int i = MDirList.count()-1;i >= 0;i--)
    //            {
    //                QFileInfo MDirInfo = MDirList.at(i);
    //                if(!MDirInfo.fileName().startsWith(".") && MDirInfo.created().date() <= curDate)
    //                {
    //                    qDebug()<<tr("符合条件月文件夹：%1").arg(MDirInfo.fileName());
    //                    //遍历日文件夹
    //                    tmpMDirName = MDirInfo.fileName();
    //                    dir.setPath(dirName + "/" + tmpYDirName + "/" +tmpMDirName);
    //                    QFileInfoList DDirList = dir.entryInfoList(QDir::Dirs,QDir::Time);
    //                    int curDFileNum = DDirList.count();
    //                    for(int i = DDirList.count() - 1;i >= 0;i--)
    //                    {
    //                        QFileInfo DDirInfo = DDirList.at(i);
    //                        if(!DDirInfo.fileName().startsWith(".") && DDirInfo.created().date() <= curDate)
    //                        {
    //                            qDebug()<<tr("符合条件日文件夹：%1").arg(DDirInfo.fileName());
    //                            tmpDDirName = DDirInfo.fileName();
    //                            dir.setPath(dirName + "/" + tmpYDirName + "/" +tmpMDirName+"/"+ tmpDDirName);
    //                            QFileInfoList fileList = dir.entryInfoList(QDir::Files,QDir::Time);
    //                            qDebug()<<tr("文件：%1中xml数量：%2").arg(dir.path().arg(fileList.count()));
    //                            for(int i = fileList.count() - 1;i >= 0;i--)
    //                            {
    //                                QFile::remove(fileList.at(i).filePath());
    //                            }
    //                            dir.rmdir(DDirInfo.filePath());
    //                            curDFileNum --;
    //                            if(curDFileNum <= 2)
    //                            {
    //                                dir.rmdir(MDirInfo.filePath());
    //                                curMFileNum --;
    //                                if(curMFileNum <= 2)
    //                                {
    //                                    dir.rmdir(YDirInfo.filePath());
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}


