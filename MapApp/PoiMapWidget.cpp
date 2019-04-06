/*
 * PoiMapWidget.cpp
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#include "PoiMapWidget.h"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QDebug>

const int KMinWptDist = 4;
const int KEdgeClipping = 20;
const int KPoiIconSize = 5;
const int KPoiIconMaxTitleLength = 16;
const qreal KRouteOpacity = 0.5;
const qreal KPlacemarkOpacity = 1;

PoiMapWidget::PoiMapWidget(const QPixmap& gpsIcon, const QPixmap& defaultIcon, QWidget* parent) :
    MapWidget(parent),
    mIconGps(gpsIcon),
    mDefaultIcon(defaultIcon),
    mRoutePoints(NULL),
    mPoiSource(NULL),
    mRouteSource(NULL),
    mMousePressed(false)
{
    clearPois();
}

PoiMapWidget::~PoiMapWidget()
{
    // TODO Auto-generated destructor stub
}

void PoiMapWidget::positionUpdate(const QPointF& location)
{
    mGpsPoi->setLocation(location);

    if(isGpsVisible())
    {
        update();
    }
}

Poi* PoiMapWidget::addPoi(const QPointF& location, const QString& title)
{
    const Poi poi(location, title);
    mPoiList.append(poi);
    return &mPoiList.last();
}

Poi* PoiMapWidget::addPoi(const QPointF& location, const QPixmap& icon)
{
    const Poi poi(location, icon);
    mPoiList.append(poi);
    return &mPoiList.last();
}

void PoiMapWidget::addPois(QList<Poi>& pois, const QPixmap* icon)
{
    for(int c = 0; c < pois.count(); c++)
    {
        Poi poi(pois.at(c).location(), pois.at(c).title());
        if(icon)
            poi.setIcon(icon);
        mPoiList.append(poi);
    }
}

void PoiMapWidget::addLatLonPoi(const Poi& poi, const QPixmap* icon)
{
    //const QPointF& latLon = poi.latLon();
    //const QPoint local = adapter()->LatLonToDisplayCoordinate(latLon);

    Poi localPoi(poi.latLon(), poi.title());

    // Use only the first word of the name.
    const QStringList list = localPoi.title().split(" ");
    if(list.count() > 0)
    {
        localPoi.setTitle(list.first().left(KPoiIconMaxTitleLength));
    }

    if(icon)
    {
        localPoi.setIcon(icon);
    }

    mPoiList.append(localPoi);
}

void PoiMapWidget::addLatLonPoiList(const QList<Poi>& poiList, const QPixmap* icon)
{
    for(int c = 0; c < poiList.count(); c++)
    {
        const Poi& poi = poiList[c];
        addLatLonPoi(poi, icon);
    }
}

const Poi* PoiMapWidget::poiAt(const QPoint& location) const
{
    for(int c = 0; mPoiSource != NULL && c < mPoiSource->poiList().count(); c++)
    {
        const Poi* poi = &mPoiSource->poiList()[c];
        const QPoint displayLocation = latLonToScreenCoordinate(poi->latLon()) / mZoom;
        const QRect poiRect(displayLocation.x() - KPoiIconSize / 2, displayLocation.y() - KPoiIconSize / 2,
                            KPoiIconSize, KPoiIconSize);
        if(poiRect.contains(location))
        {
            return poi;
        }
    }

    return NULL;
}

void PoiMapWidget::clearPois()
{
    mPoiList.clear();
    mGpsPoi = addPoi(QPointF(), mIconGps);
}

void PoiMapWidget::paintEvent(QPaintEvent* event)
{
    MapWidget::paintEvent(event);

    if(mMousePressed)
    {
        // Don't draw routes or placemarks in order to improve performance.
        return;
    }
    
    QPainter painter(this);
    QPen pen(Qt::blue);
    pen.setWidth(6);
    painter.setPen(pen);

    // Draw GPS route // TODO: move?
    for(int c = 0; c < mRoutePoints->count() - 1; c++)
    {
        const QPointF& latLon1 = mRoutePoints->at(c);
        const QPointF& latLon2 = mRoutePoints->at(c + 1);
        const QPoint wpt1 = latLonToScreenCoordinate(latLon1) / mZoom;
        const QPoint wpt2 = latLonToScreenCoordinate(latLon2) / mZoom;
        if(rect().contains(wpt1) || rect().contains(wpt2))
        {
            painter.drawLine(wpt1, wpt2);
        }
    }

    // Draw GPX routes
    painter.setOpacity(KRouteOpacity);
    for(int c = 0; mRouteSource != NULL && c < mRouteSource->routeList().count(); c++)
    {
        drawRoute(painter, mRouteSource->routeList()[c]);
    }

    pen = QPen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);

    // Draw placemarks from POI source.
    painter.setOpacity(KPlacemarkOpacity);
    for(int c = 0; mPoiSource != NULL && c < mPoiSource->poiList().count(); c++)
    {
        drawPoi(painter, mPoiSource->poiList()[c]);
    }

    // Draw POIs (those added last are drawn first, i.e. they are at the bottom of Z-order).
    for(int c = mPoiList.count() - 1; c >= 0; --c)
    {
        const QString& title = mPoiList[c].title();
        const QPoint screenCoordinate = latLonToScreenCoordinate(mPoiList[c].latLon()) / mZoom;

        if(rect().contains(screenCoordinate) & !mPoiList[c].latLon().isNull())
        {
            const QPixmap* icon = mPoiList[c].icon();
            if(icon)
            {
                // Draw POI icon if it has one.
                painter.drawPixmap(screenCoordinate.x() - icon->size().width() / 2,
                                   screenCoordinate.y() - icon->size().height() / 2, *icon);
            }
            else
            {
                // Draw POI rectangle instead of icon
                painter.drawRect(screenCoordinate.x() - KPoiIconSize, screenCoordinate.y() - KPoiIconSize,
                                 2 * KPoiIconSize, 2 * KPoiIconSize);
                painter.drawText(screenCoordinate.x() + KPoiIconSize * 2, screenCoordinate.y(), title);
            }
        }
    }

    // Draw GPS location direction arrow.
    if(!isGpsVisible() && !mGpsPoi->latLon().isNull())
    {
        const QPoint screenCoordinate = latLonToScreenCoordinate(mGpsPoi->latLon()) / mZoom;

        // Pen
        QPen pen(Qt::blue);
        pen.setWidth(3);
        painter.setPen(pen);

        // Clipping
        const QRegion clip(rect().adjusted(KEdgeClipping, KEdgeClipping,
                                               -KEdgeClipping, -KEdgeClipping));
        QRegion region(rect());
        region = region.subtracted(clip);
        painter.setClipRegion(region);

        // Draw the line
        painter.drawLine(rect().center(), screenCoordinate);
    }
}

void PoiMapWidget::mousePressEvent(QMouseEvent* event)
{
    MapWidget::mousePressEvent(event);
    mMousePressed = true;
}

void PoiMapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    MapWidget::mouseReleaseEvent(event);
    mMousePressed = false;
    update();
}

void PoiMapWidget::drawPoi(QPainter& painter, const Poi& poi)
{
    const QString& title = poi.title();
    const QPoint screenCoordinate = latLonToScreenCoordinate(poi.latLon()) / mZoom;

    if(rect().contains(screenCoordinate) & !poi.latLon().isNull())
    {
        const QPixmap* icon = poi.icon();
        if(icon == NULL)
        {
            icon = &mDefaultIcon;
        }

        QRect titleBackground = painter.fontMetrics().boundingRect(title);
        QPoint p(screenCoordinate.x() + KPoiIconSize * 2, screenCoordinate.y());
        titleBackground.translate(p);
        titleBackground.adjust(-5, -5, 5, 5);
        painter.fillRect(titleBackground, Qt::white);
        painter.drawRect(titleBackground);
        painter.drawText(p.x(), p.y(), title);

        painter.drawPixmap(screenCoordinate.x() - icon->size().width() / 2,
                           screenCoordinate.y() - icon->size().height() / 2, *icon);
    }
}

void PoiMapWidget::drawRoute(QPainter& painter, const Route& route)
{
    QPolygon polygon(route.points().length());

    for(int c = 0; c < route.points().length(); c++)
    {
        const QPoint point = latLonToScreenCoordinate(route.points()[c]) / mZoom;
        polygon.setPoint(c, point);
    }

    painter.drawPolyline(polygon);
}

QPoint PoiMapWidget::latLonToScreenCoordinate(const QPointF& latLon) const
{
    const QPoint displayLocation = adapter()->LatLonToDisplayCoordinate(latLon);
    const QPoint screenCoordinate = displayLocation - location();
    return screenCoordinate;
}

bool PoiMapWidget::isGpsVisible() const
{
    const QPoint screenCoordinateGps = latLonToScreenCoordinate(mGpsPoi->latLon()) / mZoom;
    return !mGpsPoi->latLon().isNull() && rect().contains(screenCoordinateGps);
}

void PoiMapWidget::setRoutePoints(const QList<QPointF>& routePoints)
{
    mRoutePoints = &routePoints;
}

void PoiMapWidget::setPoiSource(const IPoiSource* poiSource)
{
    mPoiSource = poiSource;
}

void PoiMapWidget::setRouteSource(const IRouteSource* routeSource)
{
    mRouteSource = routeSource;
}
