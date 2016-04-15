#ifndef CFGLAZYINITIALIZER_H
#define CFGLAZYINITIALIZER_H

#include <QList>
#include <functional>

class CfgLazyInitializer
{
    public:
        CfgLazyInitializer(std::function<void(void)> initFunc, const char* name);

        static void init();

    private:
        void doInitialize();

        std::function<void(void)> initFunc;

        static QList<CfgLazyInitializer*>* instances;
};

#endif // CFGLAZYINITIALIZER_H
