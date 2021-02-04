
# DEFINES += LICENSE_NOT_REQUIRED
DEFINES += LICENSE_NOT_REQUIRED

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

QMAKE_EXTRA_TARGETS += version

CONFIG += qt \
        thread \
        c++11

QT += network
QT -= testlib

DESTDIR = out
MOC_DIR = tmp/moc
UI_DIR = tmp/ui

INCLUDEPATH += tmp/ui


win32: LIBS += -static-libgcc -static-libstdc++
win32: LIBS += -L$$PWD/./ -lpaho-mqtt3c

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.


CONFIG(debug, debug|release) {
        TARGET = batterywatch.debug
        OBJECTS_DIR = tmp/debug
        RCC_DIR = tmp/debug/rcc
}
CONFIG(release, debug|release) {
        TARGET = batterywatch
        OBJECTS_DIR = tmp/release
        RCC_DIR = tmp/release/rcc
        QT -= testlib
}

unix {
}

# Define how to create version.h
win32 {
	version.target = version.h
	equals(QMAKE_CXX, g++):version.commands = versionit.bat
	!equals(QMAKE_CXX, g++):version.commands = ./subwcrev.sh "version.tmpl" "version.h"
	version.depends = FORCE
}



FORMS += batterywatch.ui \
    preferences.ui \

HEADERS = batterywatch.h \
    preferences.h \
	MQTTClient.h \
	MQTTClientPersistence.h \
	MQTTProperties.h \
	MQTTReasonCodes.h \
	MQTTSubscribeOpts.h \
    wemoclient.h \

SOURCES = main.cpp \
    batterywatch.cpp \
    preferences.cpp \
    wemoclient.cpp \

RESOURCES = batterywatch.qrc

