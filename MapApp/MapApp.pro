QT += core gui xml network widgets positioning

TARGET = TrailMap
TEMPLATE = app

INCLUDEPATH += proj-4.8.0/src

SOURCES += main.cpp mainwindow.cpp \
    PoiMapWidget.cpp \
    PoiListWidget.cpp \
    Poi.cpp \
    MapWidget.cpp \
    LoggerWidget.cpp \
    Logger.cpp \
    GpxWriter.cpp \
    GpxParser.cpp \
    #Coordinates.cpp \
    utility.cpp \
    mapapplication.cpp \
    settings.cpp \
    controller.cpp \
    #karttapaikkamapadapter.cpp \
    #maanmittauslaitosmapadapter.cpp \
    #mapinfotabadapter.cpp \
    imapadapter.cpp \
    LocationSource.cpp \
    LmxParser.cpp \
    #OpenStreetMapCompatibleAdapter.cpp \
    #OpenStreetMapAdapter.cpp \
    document.cpp \
    route.cpp \
    tab/Tab.cpp \
    #tab/TiffSplit.cpp \
    #tab/Tfw.cpp \
    pluginadapter.cpp
HEADERS += mainwindow.h \
    PoiMapWidget.h \
    PoiListWidget.h \
    Poi.h \
    MapWidget.h \
    LoggerWidget.h \
    Logger.h \
    GpxWriter.h \
    GpxParser.h \
    utility.h \
    mapapplication.h \
    settings.h \
    controller.h \
    imapadapter.h \
    #karttapaikkamapadapter.h \
    #maanmittauslaitosmapadapter.h \
    #mapinfotabadapter.h \
    LocationSource.h \
    config.h \
    LmxParser.h \
    #OpenStreetMapCompatibleAdapter.h \
    #OpenStreetMapAdapter.h \
    document.h \
    IPoiSource.h \
    IRouteSource.h \
    route.h \
    tab/Tab.h \
    #tab/TiffSplit.h \
    #tab/Tfw.h \
    MapPluginInterface.h \
    pluginadapter.h

FORMS += mainwindow.ui \

RESOURCES += \
    resources.qrc

DEPENDPATH += proj-4.8.0


CONFIG += mobility
MOBILITY =
