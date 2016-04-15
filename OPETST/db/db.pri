INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

HEADERS += \
    $$PWD/db.h \
    $$PWD/sqlquery.h \
    $$PWD/dbobjecttype.h \
    $$PWD/sqlresultsrow.h \
    $$PWD/sqlerrorcodes.h \
    $$PWD/abstractdb3.h \
    $$PWD/dbsqlite3.h \
    $$PWD/log.h \
    $$PWD/dbmanager.h \
    $$PWD/abstractdb.h \
    $$PWD/interruptable.h \
    $$PWD/attachguard.h \
    $$PWD/asyncqueryrunner.h

SOURCES += \
    $$PWD/sqlquery.cpp \
    $$PWD/db.cpp \
    $$PWD/sqlresultsrow.cpp \
    $$PWD/sqlerrorcodes.cpp \
    $$PWD/dbsqlite3.cpp \
    $$PWD/log.cpp \
    $$PWD/dbmanager.cpp \
    $$PWD/abstractdb.cpp \
    $$PWD/attachguard.cpp \
    $$PWD/asyncqueryrunner.cpp
