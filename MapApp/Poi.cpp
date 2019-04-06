/*
 * Poi.cpp
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#include "Poi.h"

#include <QDebug>

Poi::Poi() :
    mIcon(NULL)
{
}

Poi::Poi(const QPointF& location, const QString& title) :
    mLatLon(location), mTitle(title), mIcon(NULL)
{
}

Poi::Poi(const QPoint& location, const QString& title) :
    mLocation(location), mTitle(title), mIcon(NULL)
{
}

Poi::Poi(const QPointF& location, const QPixmap& icon, const QString& title)
    : mLatLon(location), mTitle(title)
{
    mIcon = &icon;
}

Poi::Poi(const Poi& poi)
{
    mLatLon = poi.mLatLon;
    mLocation = poi.mLocation;
    mTitle = poi.mTitle;
    mIcon = poi.mIcon;
}

void Poi::setLocation(const QPoint& location)
{
    mLocation = location;
}

const QPoint& Poi::location() const
{
    return mLocation;
}

void Poi::setLocation(const QPointF& location)
{
    mLatLon = location;
}

const QPointF& Poi::latLon() const
{
    return mLatLon;
}

void Poi::setTitle(const QString& title)
{
    mTitle = title;
}

const QString& Poi::title() const
{
    return mTitle;
}

void Poi::setIcon(const QPixmap* icon)
{
    mIcon = icon;
}

const QPixmap* Poi::icon() const
{
    return mIcon;
}

bool Poi::operator ==(const Poi& other)
{
    return &other == this;
}
