#include "pluginadapter.h"

PluginAdapter::PluginAdapter(IMapPluginInterface& plugin, const QString& name, QObject* parent) :
    mPlugin(plugin),
    IMapAdapter(parent)
{
    mAdapterName = name;
    mTileSize = mPlugin.tileSize();
}

bool PluginAdapter::openDataSource(QString& path)
{
    QString workingFolder = path;
    if(!workingFolder.endsWith("/"))
    {
        workingFolder.append("/");
    }

    mPlugin.setWorkingFolder(workingFolder);
    mWorkingFolder = workingFolder;

    mMinZoom = mPlugin.minZoom();
    mMaxZoom = mPlugin.maxZoom();

    return true;
}

QPoint PluginAdapter::LatLonToDisplayCoordinate(const QPointF& coordinate)
{
    return mPlugin.LatLonToDisplayCoordinate(coordinate);
}

QPointF PluginAdapter::DisplayCoordinateToLatLon(const QPoint& coordinate)
{
    return mPlugin.DisplayCoordinateToLatLon(coordinate);
}

QPixmap PluginAdapter::loadTile(const QPoint& tile)
{
    return mPlugin.loadTile(tile);
}

QPoint PluginAdapter::defaultLocation() const
{
    return mPlugin.defaultLocation();
}

IMapPluginInterface& PluginAdapter::plugin()
{
    return mPlugin;
}

void PluginAdapter::importProgress(int value)
{
    emit progress(value);
}

void PluginAdapter::importCompleted()
{
    emit openCompleted();
}

void PluginAdapter::mapDataChanged()
{
    emit dataChanged();
}

void PluginAdapter::mapDataInvalidated()
{
    emit invalidate();
}

QString PluginAdapter::dataFolder()
{
    return mWorkingFolder;
}

void PluginAdapter::setZoom(int zoom)
{
    mPlugin.setZoom(zoom);
}

int PluginAdapter::zoom() const
{
    return mPlugin.zoom();
}
