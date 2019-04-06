/*
 * Poi.h
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#ifndef POI_H_
#define POI_H_

#include <QPoint>
#include <QString>
#include <QPixmap>

class Poi
{
public:
    Poi();
    Poi(const QPointF& location, const QString& title = QString());
    Poi(const QPoint& location, const QString& title = QString());
    Poi(const QPointF& location, const QPixmap& icon, const QString& title = QString());
    Poi(const Poi& poi);
    
public:
    /*
     * Location is in local coordinates (pixels).
     **/
    void setLocation(const QPoint& location);
    const QPoint& location() const;

    void setLocation(const QPointF& location);
    const QPointF& latLon() const;
    
    void setTitle(const QString& title);
    const QString& title() const;

    void setIcon(const QPixmap* icon);
    const QPixmap* icon()  const;

    bool operator ==(const Poi&);
    
private:
    QPointF mLatLon;
    QPoint mLocation;
    QString mTitle;
    const QPixmap* mIcon;
};

#endif /* POI_H_ */
