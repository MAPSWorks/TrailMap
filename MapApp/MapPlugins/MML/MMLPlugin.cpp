#include "MMLPlugin.h"

#include "Coordinates.h"

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDebug>

const QString KFileNameFormat("%1/%2/%3.png");

MMLPlugin::MMLPlugin() :
    QObject(),
    mImporter(NULL),
    mZoomLevel(minZoom())
{
    mProjWgs84 = pj_init_plus("+proj=latlong +datum=WGS84 +ellps=WGS84");
    mProjEtrstm35Fin = pj_init_plus("+proj=utm +zone=35 +ellps=GRS80 +units=m +no_defs");

    // Default tile size.
    mTileSize = QSize(480, 480);
}

MMLPlugin::~MMLPlugin()
{
    pj_free(mProjEtrstm35Fin);
    pj_free(mProjWgs84);
}

void MMLPlugin::setWorkingFolder(const QString& path)
{
    mWorkingFolder = path;
    if(!mWorkingFolder.endsWith("/"))
    {
        mWorkingFolder.append("/");
    }

    // Find available map levels.
    scanMapLevels();

    // Find default file.
    const QString tilesFile = findDefaultFile();
    if(!tilesFile.isEmpty())
    {
        readTilesFile(tilesFile);
    }
}

void MMLPlugin::openDataSource(QUrl&)
{

}

void MMLPlugin::importData(QUrl&)
{

}

void MMLPlugin::createMenu(QMenu& menu)
{
    Q_ASSERT(mImporter == NULL);
    mImporter = new Importer(mWorkingFolder, mTileSize, this);
    connect(mImporter, SIGNAL(progress(int)), this, SLOT(importProgress(int)));
    connect(mImporter, SIGNAL(done()), this, SLOT(importDone()));

    QAction* actionImport = menu.addAction(tr("Import data..."));
    connect(actionImport, SIGNAL(triggered()), mImporter, SLOT(import()));
}

QPoint MMLPlugin::LatLonToDisplayCoordinate(const QPointF& coordinate) const
{
    QPoint result;

    double x = KKJCoordinates::DegreeToRadian(coordinate.x());
    double y = KKJCoordinates::DegreeToRadian(coordinate.y());

    int res = pj_transform(mProjWgs84, mProjEtrstm35Fin, 1, 1, &x, &y, NULL);

    if(res == 0)
    {
        // Convert to local pixel coordinate.
        // First bias to origo.
        const QPoint KWorldOrigo(20006, 7817994);
        result = QPoint(QPoint(x, y) - KWorldOrigo);
        result.setY(result.y() * -1);

        // Convert to tile.
        const qreal fPixelX = (qreal) (result.x() / mSourceMapScale) / (1200 * mTileScale);
        const qreal fPixelY = (qreal) (result.y() / mSourceMapScale) / (1200 * mTileScale);

        // Convert to local pixel
        result = QPoint(fPixelX * mTileSize.width(), fPixelY * mTileSize.height());
    }

    return result;
}

QPointF MMLPlugin::DisplayCoordinateToLatLon(const QPoint& coordinate) const
{
    QPointF a(coordinate.x() / (qreal)mTileSize.width(),
              coordinate.y() / (qreal)mTileSize.height());

    QPoint result(a.x() * (1200 * mTileScale) * mSourceMapScale,
                  a.y() * (1200 * mTileScale) * mSourceMapScale);

    result.setY(result.y() * -1);

    const QPoint KWorldOrigo(20006, 7817994);       // TODO:
    result += KWorldOrigo;
    // result is Etrstm35Fin.

    double x = result.x(), y = result.y();
    int res = pj_transform(mProjEtrstm35Fin, mProjWgs84, 1, 1, &x, &y, NULL);
    if(res == 0)
    {
        return QPointF(KKJCoordinates::RadianToDegree(x),
                       KKJCoordinates::RadianToDegree(y));
    }

    return QPointF();       // TODO:
}

QSize MMLPlugin::tileSize() const
{
    return mTileSize;
}

QPixmap MMLPlugin::loadTile(const QPoint& tile)
{
    const QString filePath(mWorkingFolder + KFileNameFormat.arg(mapLevel()).arg(tile.x()).arg(tile.y()));

    QPixmap pixmap;
    pixmap.load(filePath);
    return pixmap;
}

QPoint MMLPlugin::defaultLocation() const
{
    return mDefaultLocation;
}

int MMLPlugin::minZoom() const
{
    return 1;
}

int MMLPlugin::maxZoom() const
{
    // Return value is >= 0.
    return qMax(mMapLevelsList.count(), 1);
}

int MMLPlugin::zoom() const
{
    return mZoomLevel;
}

void MMLPlugin::setZoom(int zoom)
{
    mZoomLevel = zoom;

    // TODO: read params from .tiles file.

    // Find default file.
    const QString tilesFile = findDefaultFile();
    if(!tilesFile.isEmpty())
    {
        readTilesFile(tilesFile);
    }
}

QPoint MMLPlugin::defaultLocation(const QString& tilesFile) const
{
    const QString ptLeft = getXmlParam(tilesFile, "Left");
    const QString ptTop = getXmlParam(tilesFile, "Top");
    const QString ptRight= getXmlParam(tilesFile, "Right");
    const QString ptBottom = getXmlParam(tilesFile, "Bottom");

    const QRect mapArea(QPoint(ptLeft.toInt(), ptTop.toInt()), QPoint(ptRight.toInt(), ptBottom.toInt()));

    return mapArea.center();
}

QString MMLPlugin::getXmlParam(const QString& fileName, const QString& paramName) const
{
    QString result;

    QDomDocument doc("Tiles");
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        if(doc.setContent(&file))
        {
            QDomElement elementDoc = doc.documentElement();
            QDomNode node = elementDoc.firstChild();
            while(!node.isNull())
            {
                QDomElement element = node.toElement();
                if(!element.isNull())
                {
                    if(element.tagName() == paramName)
                    {
                        result = element.text();
                        break;
                    }
                }

                node = node.nextSibling();
            }
        }
        file.close();
    }

    return result;
}

QSize MMLPlugin::getTileSize(const QString& fileName) const
{
    QSize tileSize(getXmlParam(fileName, "TileWidth").toInt(),
                   getXmlParam(fileName, "TileHeight").toInt());
    if(tileSize.isNull())
    {
        int size = getXmlParam(fileName, "TileSize").toInt();
        tileSize = QSize(size, size);
    }

    return tileSize;
}

QString MMLPlugin::mapLevel() const
{
    if(mMapLevelsList.count() > 0)
    {
        return mMapLevelsList.at(zoom() - 1);
    }
    else
    {
        return QString();
    }
}

// Find the first .tiles file in working folder for current map/zoom level.
QString MMLPlugin::findDefaultFile() const
{
    QDir dir(mWorkingFolder + mapLevel());
    QStringList filters;
    filters.append("*.tiles");
    dir.setNameFilters(filters);
    QStringList entryList = dir.entryList();
    if(entryList.count() > 0)
        return entryList.first();
    return QString();
}

void MMLPlugin::readTilesFile(const QString& fileName)
{
    // TODO: use proper default values or report error in case tilesFile is invalid.
    qDebug() << "Reading tiles file:" << fileName;

    const QString tilesFile = mWorkingFolder + mapLevel() + "/" + fileName;
    mTileSize = getTileSize(tilesFile);
    mDefaultLocation = defaultLocation(tilesFile);
    mSourceMapScale = qMax(getXmlParam(tilesFile, "SourceMapScale").toInt(), 1);
    mTileScale = getXmlParam(tilesFile, "Scale").toInt();

    qDebug() << "Tile size:" << mTileSize;
    qDebug() << "Default location:" << mDefaultLocation;
    qDebug() << "Map scale:" << mSourceMapScale;
    qDebug() << "Tile scale:" << mTileScale;
}

void MMLPlugin::scanMapLevels()
{
    QDir mapLevelsDir(mWorkingFolder);
    QStringList folderList = mapLevelsDir.entryList();
    folderList.sort();

    foreach(QString folder, folderList)
    {
        bool ok;
        const int mapLevel = folder.toInt(&ok);
        if(ok)
        {
            mMapLevelsList.insert(0, QString::number(mapLevel));
        }
    }
}

void MMLPlugin::importProgress(int value)
{
    emit progress(value);
}

void MMLPlugin::importDone()
{
    emit done();
}


