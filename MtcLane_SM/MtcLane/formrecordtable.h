#ifndef FORMRECORDTABLE_H
#define FORMRECORDTABLE_H

#include <QWidget>
#include <QDateTime>
namespace Ui {
    class FormRecordTable;
}

class FormRecordTable : public QWidget
{
    Q_OBJECT

public:
    explicit FormRecordTable(QWidget *parent = 0);
    virtual ~FormRecordTable();
    void insertEntryInfo(const QDateTime& iBusTime, const QString& sCardNum,
                         const QString& sVehType, const QString& sVehPlate);
    void insertExitInfo(const QDateTime& iBusTime, const QString& sCardNum,
                        const QString& sVehType, const QString& sVehPlate, const QString& sEnStation,
                        double dMoney, double dFinal, double dRest);
    void setCardAndVehNum(int card,int veh);
    void clearExitInfo();
    void ReverseSet();
private:
    Ui::FormRecordTable *ui;
    QStringList getEntryHeader();
    QStringList getExitHeader();
};

#endif // FORMRECORDTABLE_H
