#ifndef USERINPUTFILTER_H
#define USERINPUTFILTER_H

#include <QObject>

class QTimer;
class QLineEdit;

class UserInputFilter : public QObject
{
        Q_OBJECT

    public:
        UserInputFilter(QLineEdit* lineEdit, QObject* filterHandler, const char* handlerSlot);

        void setDelay(int msecs);

    private:
        QTimer* timer = nullptr;
        QLineEdit* lineEdit = nullptr;

    private slots:
        void filterModified(const QString& newValue);
        void applyFilter();

    signals:
        void applyFilter(const QString& value);
};

#endif // USERINPUTFILTER_H
