QT       += core gui
#QT       += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./netapi/netapi.pri)

INCLUDEPATH += ./netapi/

SOURCES += \
    ImageWidget.cpp \
    PacketQueue.cpp \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    mediaplayerdialog.cpp \
    qmymovielabel.cpp \
    registerdialog.cpp \
    uploaddialog.cpp \
    videoplayer.cpp

HEADERS += \
    ImageWidget.h \
    PacketQueue.h \
    ckernel.h \
    logindialog.h \
    mediaplayerdialog.h \
    qmymovielabel.h \
    registerdialog.h \
    uploaddialog.h \
    videoplayer.h

FORMS += \
    logindialog.ui \
    mediaplayerdialog.ui \
    qmymovielabel.ui \
    registerdialog.ui \
    uploaddialog.ui

include(./opengl/opengl.pri)
INCLUDEPATH += ./opengl/

INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include\
                $$PWD/SDL2-2.0.10/include
#LIBS += -lopengl32
LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib\
        $$PWD/ffmpeg-4.2.2/lib/avdevice.lib\
        $$PWD/ffmpeg-4.2.2/lib/avfilter.lib\
        $$PWD/ffmpeg-4.2.2/lib/avformat.lib\
        $$PWD/ffmpeg-4.2.2/lib/avutil.lib\
        $$PWD/ffmpeg-4.2.2/lib/postproc.lib\
        $$PWD/ffmpeg-4.2.2/lib/swresample.lib\
        $$PWD/ffmpeg-4.2.2/lib/swscale.lib\
        $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
