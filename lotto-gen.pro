TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += jsoncpp curlpp

DEFINES += _GLIBCXX_USE_CXX11_ABI
SOURCES += \
        main.cpp
LIBS += -lleveldb
