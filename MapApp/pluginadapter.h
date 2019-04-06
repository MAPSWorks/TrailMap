#ifndef PLUGINADAPTER_H
#define PLUGINADAPTER_H

#include "IMapAdapter.h"

#include "MapPluginInterface.h"

class PluginAdapter : public IMapAdapter
{
    Q_OBJECT
public:
    PluginAdapter(IMapPluginInterface& plugin, const QString& name, QObject* parent);

public:     // From IMapAdapter
    bool openDataSource(QString& path);
    QPoint LatLonToDisplayCoordinate(const QPointF& coordinate);
    QPointF DisplayCoordinateToLatLon(const QPoint& coordinate);
    QPixmap loadTile(const QPoint& tile);
    QPoint defaultLocation() const;
    QString dataFolder();
    void setZoom(int zoom);
    int zoom() const;

public:     // New methods
    IMapPluginInterface& plugin();

public slots:
    void importProgress(int);
    void importCompleted();
    void mapDataChanged();
    void mapDataInvalidated();

private:
    IMapPluginInterface& mPlugin;
    QString mWorkingFolder;
};

#endif // PLUGINADAPTER_H
