#ifndef LOCATIONSOURCE_H
#define LOCATIONSOURCE_H

#include "config.h"

#include <QObject>
#include <QPointF>

#if defined(MAP_ON_MOBILE)
    #include <QGeoPositionInfoSource>

#else
    class QGeoPositionInfo
    {
        // Dummy class for desktop compatibility.
    };

    class QGeoPositionInfoSource
    {
        // Dummy class for desktop compatibility.
    };
#endif


class LocationSource : public QObject
{
    Q_OBJECT

public:
    LocationSource();

    QPointF location() const;

signals:
    void newLocation(QPointF);

public slots:
    void positionUpdated(const QGeoPositionInfo& update);

private:
    QGeoPositionInfoSource* mPositioningSource;

private:
    QPointF mLocation;
};

#endif // LOCATIONSOURCE_H
