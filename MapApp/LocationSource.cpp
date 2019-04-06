#include "LocationSource.h"

#include <QDebug>

#if defined(MAP_ON_MOBILE)
    #include <QGeoPositionInfo>

    const int KUpdateInterval = 1*1000;

#endif

LocationSource::LocationSource()
{
#if defined(MAP_ON_MOBILE)
    mPositioningSource = QGeoPositionInfoSource::createDefaultSource(this);
    if(mPositioningSource)
    {
        mPositioningSource->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
        connect(mPositioningSource, SIGNAL(positionUpdated(const QGeoPositionInfo&)),
                              this, SLOT(positionUpdated(const QGeoPositionInfo&)));
        mPositioningSource->setUpdateInterval(KUpdateInterval);
        mPositioningSource->startUpdates();

        QGeoPositionInfo pi = mPositioningSource->lastKnownPosition();
        if(pi.isValid())
        {
            mLocation = QPointF(pi.coordinate().longitude(), pi.coordinate().latitude());
        }
    }
    else
    {
        qDebug() << "QGeoPositionInfoSource not created!";
    }
#endif
}

QPointF LocationSource::location() const
{
    return mLocation;
}

void LocationSource::positionUpdated(const QGeoPositionInfo& update)
{
#if defined(MAP_ON_MOBILE)
    if(update.isValid())
    {
        QPointF location(update.coordinate().longitude(), update.coordinate().latitude());
        mLocation = location;
        emit newLocation(location);
    }
#endif
}
