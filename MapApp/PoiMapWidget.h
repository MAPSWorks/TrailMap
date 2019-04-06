/*
 * PoiMapWidget.h
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#ifndef POIMAPWIDGET_H_
#define POIMAPWIDGET_H_

#include "MapWidget.h"
#include "Poi.h"
#include "IPoiSource.h"
#include "IRouteSource.h"

#include <QPixmap>
#include <QList>
#include <QPointF>

class PoiMapWidget : public MapWidget
{
public:
    PoiMapWidget(const QPixmap& gpsIcon, const QPixmap& defaultIcon, QWidget* parent = NULL);
    virtual ~PoiMapWidget();
    
public:
    // Update position with geographical coordinates
    void positionUpdate(const QPointF& location);

    void setPoiSource(const IPoiSource* poiSource);
    void setRouteSource(const IRouteSource* routeSource);
    Poi* addPoi(const QPointF& location, const QString& title = QString());
    Poi* addPoi(const QPointF& location, const QPixmap& icon);
    void addPois(QList<Poi>& pois, const QPixmap* icon = NULL);
    void addLatLonPoi(const Poi& poi, const QPixmap* icon = NULL);
    void addLatLonPoiList(const QList<Poi>& pois, const QPixmap* icon = NULL);
    const Poi* poiAt(const QPoint& location) const;

    void clearPois();
    //void clearRoute();

    bool isGpsVisible() const;

    void setRoutePoints(const QList<QPointF>& routePoints);
    
protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void drawPoi(QPainter& painter, const Poi& poi);
    void drawRoute(QPainter& painter, const Route& route);
    QPoint latLonToScreenCoordinate(const QPointF& latLon) const;

private:
    QList<Poi> mPoiList;    // Pixel coordinates
    const IPoiSource* mPoiSource;
    const IRouteSource* mRouteSource;
    const QList<QPointF>* mRoutePoints;

    const QPixmap& mIconGps;
    const QPixmap& mDefaultIcon;
    Poi* mGpsPoi;   // BYREF: GPS location indicator
    bool mMousePressed;
};

#endif /* POIMAPWIDGET_H_ */
