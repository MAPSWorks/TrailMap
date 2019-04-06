/*
 * Logger.cpp
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#include "Logger.h"
#include "Utility.h"

//#include <QGeoCoordinate>

QTM_USE_NAMESPACE

Logger::Logger() : 
    mDistance(0.0)
{
    mStartTime = QDateTime::currentDateTime();
}

void Logger::addLocation(const QPointF& latlon)
{/*
    if(mLatLonList.count() > 0)
    {
        const QGeoCoordinate coordinate1(latlon.y(), latlon.x());
        const QGeoCoordinate coordinate2(mLatLonList.last().y(), mLatLonList.last().x());
        const qreal distance = coordinate1.distanceTo(coordinate2);
        mDistance += distance;
    }

    mLatLonList.append(latlon);*/
}

qreal Logger::distance() const
{
    return mDistance;
}

int Logger::duration() const
{
    if(mEndTime.isNull())
        return mStartTime.secsTo(QDateTime::currentDateTime());
    else
        return mStartTime.secsTo(mEndTime);
}

qreal Logger::averageVelocity() const
{
    const qreal d = distance();
    return d / duration();
}

void Logger::stop()
{
    if(mEndTime.isNull())
        mEndTime = QDateTime::currentDateTime();
}
