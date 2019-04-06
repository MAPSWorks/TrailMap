/*
 * Logger.h
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <QList>
#include <QPointF>
#include <QDateTime>

class Logger
{
public:
    Logger();
    
public:
    void addLocation(const QPointF& latlon);
    
public:
    /**
     * Returns the logged distance in metres. 
     */
    qreal distance() const;
    
    /**
     * The duration of the log in seconds.
     */
    int duration() const;
    
    /**
     * Average velocity in m/s.
     */
    qreal averageVelocity() const;

    /**
     * Stops the logger.
     */
    void stop();
    
private:
    QDateTime mStartTime;
    QDateTime mEndTime;
    QList<QPointF> mLatLonList;
    qreal mDistance;
};

#endif /* LOGGER_H_ */
