#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QList>
#include "Poi.h"
#include "IPoiSource.h"
#include "IRouteSource.h"

class Document : public IPoiSource, public IRouteSource
{
public:
    Document();
    Document(const QList<Poi>& poiList, const QList<Route>& routeList, const QString& filename);
    const QList<Poi>& placemarks() const;
    bool hasChanged() const;
    QString filename() const;

public: // Modification
    void addPlacemark(const Poi& placemark);
    void removePlacemark(const Poi& placemark);

public: // From IPoiSource
    const QList<Poi>& poiList() const;

public: // From IRouteSource
    const QList<Route>& routeList() const;

private:
    QString mFilename;
    bool mModified;
    QList<Poi> mPoiList;
    QList<Route> mRouteList;
};

#endif // DOCUMENT_H
