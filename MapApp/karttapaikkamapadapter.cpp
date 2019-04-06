#include "karttapaikkamapadapter.h"

#include "Coordinates.h"
#include "utility.h"

#include <QFileDialog>
#include <QDebug>

const QString KAdapterName("Karttapaikka");
const QString KFileNameFormat("%1/map%2x%3");

KarttapaikkaMapAdapter::KarttapaikkaMapAdapter(QObject* parent) :
    IMapAdapter(parent)
{
    setDataFolder(Utility::findDataFolder(KAdapterName));

    // Read tile size from file or use default.
    const QPixmap pixmap = loadTile(QPoint(0, 0));  // TODO: find some tile.
    mTileSize = pixmap.size();
    if(mTileSize.isNull())
    {
        // Use default tile size if the file could not be found.
        mTileSize = QSize(480, 480);
    }
}

QString KarttapaikkaMapAdapter::name()
{
    return KAdapterName;
}

IMapAdapter::TDataSourceType KarttapaikkaMapAdapter::DataSourceType()
{
    return Directory;
}

bool KarttapaikkaMapAdapter::openDataSource(QString& folder)
{
    //const QString folder = QFileDialog::getExistingDirectory(NULL, tr("Select map folder:"));
    if(!folder.isEmpty())
    {
        qDebug() << "Data folder:" << folder;
        setDataFolder(folder);
        emit openCompleted();
    }
    return !folder.isEmpty();
}

QPoint KarttapaikkaMapAdapter::LatLonToDisplayCoordinate(const QPointF& coordinate)
{
    return KKJCoordinates::LatLonToPixelL(coordinate.y(), coordinate.x());
}

QPointF KarttapaikkaMapAdapter::DisplayCoordinateToLatLon(const QPoint& coordinate)
{
    // TODO:
    return QPointF();
}

QPixmap KarttapaikkaMapAdapter::loadTile(const QPoint& tile)
{
    const QString filePath(mDataFolder + KFileNameFormat.arg(tile.x()).arg(tile.x()).arg(tile.y()));

    QStringList extensions;
    extensions.append(".dat");
    extensions.append(".png");

    for(int ext = 0; ext < extensions.count(); ext++)
    {
        const QString path = filePath + extensions.at(ext);

        QPixmap pixmap;
        pixmap.load(path);
        if(!pixmap.isNull())
        {
            return pixmap;
        }
    }

    return QPixmap();
}
