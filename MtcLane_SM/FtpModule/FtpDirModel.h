#ifndef FTPDIRMODEL_H
#define FTPDIRMODEL_H

#include <QAbstractItemModel>
#include <QAuthenticator>
#include <QUrl>
#include <QUrlInfo>
class FtpEntry;

struct FtpDirModelData;
class FtpDirModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FtpDirModel(QObject* parent=0);
    ~FtpDirModel();

    void setUrl(const QUrl& url);
    QUrl url() const;

    void setLogin(const QString& user,
                  const QString& password);
    QString user() const;
    QString password() const;

    QUrl url(const QModelIndex& index) const;
    QString path(const QModelIndex& index) const;
    QModelIndex index(const QString& path) const;
    static QUrlInfo getUrlInfo(const QModelIndex& index);
    static QString getDir(const QModelIndex& index);
public slots:
    void refresh(const QModelIndex& index=QModelIndex());

signals:
    void loginParamsRequest(QAuthenticator* auth);

protected:
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int col, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

    QModelIndex index(FtpEntry* entry) const;

private slots:
    void slotListingDone(FtpEntry* entry);

private:
    FtpDirModelData* d;
};

#endif // FTPDIRMODEL_H
