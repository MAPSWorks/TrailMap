#ifndef KARTTAPAIKKAPLUGIN_H
#define KARTTAPAIKKAPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include <proj_api.h>

#include "MapPluginInterface.h"

/**
  * Legacy format plugin
  *
  */
class KarttapaikkaPlugin : public QObject, public IMapPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.TrailMap.TrailMap.MapPluginInterface")
    Q_INTERFACES(IMapPluginInterface)

public:
    KarttapaikkaPlugin();
    virtual ~KarttapaikkaPlugin();

public: // Interface methods
    void setWorkingFolder(const QString& path);
    void openDataSource(QUrl&);
    void importData(QUrl&);
    void createMenu(QMenu&);

    QPoint LatLonToDisplayCoordinate(const QPointF& coordinate) const;
    QPointF DisplayCoordinateToLatLon(const QPoint& coordinate) const;
    QSize tileSize() const;
    QPixmap loadTile(const QPoint& tile);
    QPoint defaultLocation() const;
    int minZoom() const;
    int maxZoom() const;
    int zoom() const;
    void setZoom(int);

private:
    QString mWorkingFolder;
    QSize mTileSize;
};

#endif // KARTTAPAIKKAPLUGIN_H
