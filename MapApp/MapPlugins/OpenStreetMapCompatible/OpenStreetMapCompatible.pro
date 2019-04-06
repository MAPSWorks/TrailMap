TEMPLATE     = lib
CONFIG      += plugin
QT          += core gui network widgets
INCLUDEPATH += ../..
HEADERS     = OpenStreetMapCompatiblePlugin.h
SOURCES     = OpenStreetMapCompatiblePlugin.cpp

TARGET      = $$qtLibraryTarget(OsmCompatible)

# TODO: Fix for non-Android only!
!unix {
    DESTDIR     = ../../plugins
}
