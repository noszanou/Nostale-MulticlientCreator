QT       += core gui widgets

CONFIG += c++2b

TARGET = NostaleClientCreator

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/core/Binary.cpp \
    src/core/ClientBuilder.cpp \
    src/core/PeImage.cpp \
    src/platform/FileVersion.cpp \
    src/ui/MainWindow.cpp

HEADERS += \
    src/core/Binary.h \
    src/core/ClientBuilder.h \
    src/core/PeImage.h \
    src/core/Signature.h \
    src/platform/FileVersion.h \
    src/ui/MainWindow.h \
    src/ui/StyleSheet.h

RESOURCES += \
    mainform.qrc

RC_ICONS = img/logo.ico

LIBS += -lversion

msvc {
    QMAKE_LFLAGS_WINDOWS += "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
} else {
    QMAKE_MANIFEST = $$PWD/MulticlientCreator.manifest
}
