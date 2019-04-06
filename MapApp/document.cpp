#include "document.h"

Document::Document() :
    mModified(false)
{
}

Document::Document(const QList<Poi>& poiList, const QList<Route>& routeList, const QString& filename) :
    mModified(false),
    mFilename(filename)
{
    mPoiList = poiList;
    mRouteList = routeList;
}

const QList<Poi>& Document::placemarks() const
{
    return mPoiList;
}

void Document::addPlacemark(const Poi& placemark)
{
    mPoiList.append(placemark);
    mModified = true;
}

void Document::removePlacemark(const Poi& placemark)
{
    mPoiList.removeOne(placemark);
}

bool Document::hasChanged() const
{
    return mModified;
}

const QList<Poi>& Document::poiList() const
{
    return mPoiList;
}

const QList<Route>& Document::routeList() const
{
    return mRouteList;
}

QString Document::filename() const
{
    return mFilename;
}
