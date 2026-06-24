QT += core
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt

INCLUDEPATH += ../include/
LIBS += -lpthread
LIBS += -lmysqlclient
LIBS += -lcryptopp
SOURCES += \
    ../src/Mysql.cpp \
    ../src/TCPKernel.cpp \
    ../src/Thread_pool.cpp \
    ../src/block_epoll_net.cpp \
    ../src/clogic.cpp \
    ../src/err_str.cpp \
    ../src/main.cpp \
    cmymd5.cpp

HEADERS += \
    ../include/Mysql.h \
    ../include/TCPKernel.h \
    ../include/Thread_pool.h \
    ../include/block_epoll_net.h \
    ../include/clogic.h \
    ../include/err_str.h \
    ../include/packdef.h \
    cmymd5.h
