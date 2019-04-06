#ifndef MAPPLUGININTERFACE_H
#define MAPPLUGININTERFACE_H

#include <QUrl>
#include <QMenu>
#include <QPoint>

#include "imapadapter.h"


class IMapPluginInterface
{
public:
    virtual ~IMapPluginInterface() {}

    virtual void setWorkingFolder(const QString& path) = 0;
    virtual void openDataSource(QUrl&) = 0;
    virtual void importData(QUrl&) = 0;
    virtual void createMenu(QMenu&) { }

    virtual QPoint LatLonToDisplayCoordinate(const QPointF& coordinate) const = 0;
    virtual QPointF DisplayCoordinateToLatLon(const QPoint& coordinate) const = 0;
    virtual QSize tileSize() const = 0;
    virtual QPixmap loadTile(const QPoint& tile) = 0;
    virtual QPoint defaultLocation() const = 0;
    virtual int minZoom() const = 0;
    virtual int maxZoom() const = 0;
    virtual int zoom() const = 0;
    virtual void setZoom(int) = 0;

    // TODO: bookmarks ?
};

Q_DECLARE_INTERFACE(IMapPluginInterface, "org.TrailMap.TrailMap.MapPluginInterface/1.0")

#endif // MAPPLUGININTERFACE_H
