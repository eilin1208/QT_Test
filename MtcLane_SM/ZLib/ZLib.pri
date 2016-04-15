INCLUDEPATH += $$PWD/include

win32 {
        message(zlib use windows lib)
        LIBS += $$PWD/lib/windows/libzdll.a
}

unix {

    linux-g++ {
		message(zlib use linux lib)
		LIBS += $$PWD/lib/linux/libz.a
    }

    linux-EMRC-R3-g++ {
		message(zlib use arm linux lib)
		LIBS += $$PWD/lib/arm/libz.so
    }
}
