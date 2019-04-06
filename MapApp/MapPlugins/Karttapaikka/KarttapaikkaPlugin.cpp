#include "KarttapaikkaPlugin.h"

#include "Coordinates.h"

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDebug>

const QString KFileNameFormat("%1/map%2x%3.dat");

KarttapaikkaPlugin::KarttapaikkaPlugin() :
    QObject()
{
    // Default tile size.
    mTileSize = QSize(480, 480);
}

KarttapaikkaPlugin::~KarttapaikkaPlugin()
{
}

void KarttapaikkaPlugin::setWorkingFolder(const QString& path)
{
    mWorkingFolder = path;
}

void KarttapaikkaPlugin::openDataSource(QUrl&)
{

}

void KarttapaikkaPlugin::importData(QUrl&)
{

}

void KarttapaikkaPlugin::createMenu(QMenu& /*menu*/)
{
}

QPoint KarttapaikkaPlugin::LatLonToDisplayCoordinate(const QPointF& coordinate) const
{
    return KKJCoordinates::LatLonToPixelL(coordinate.y(), coordinate.x());
}

QPointF KarttapaikkaPlugin::DisplayCoordinateToLatLon(const QPoint& coordinate) const
{
    return QPointF();       // TODO:
}

QSize KarttapaikkaPlugin::tileSize() const
{
    return mTileSize;
}

QPixmap KarttapaikkaPlugin::loadTile(const QPoint& tile)
{
    const QString filePath(mWorkingFolder + KFileNameFormat.arg(tile.x()).arg(tile.x()).arg(tile.y()));

    QPixmap pixmap;
    pixmap.load(filePath);

    if(pixmap.size() != mTileSize)
    {
        // We must crop
        pixmap = pixmap.copy(0, 21, 480, 480);
    }

    return pixmap;
}

QPoint KarttapaikkaPlugin::defaultLocation() const
{
    // TODO: scan working folder
    return QPoint();
}

int KarttapaikkaPlugin::minZoom() const
{
    return 1;
}

int KarttapaikkaPlugin::maxZoom() const
{
    return minZoom();
}

int KarttapaikkaPlugin::zoom() const
{
    return minZoom();
}

void KarttapaikkaPlugin::setZoom(int /*zoom*/)
{
}

