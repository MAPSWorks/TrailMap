#include "maanmittauslaitosmapadapter.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QRect>
#include <QDebug>

#include "Coordinates.h"
#include "utility.h"

const QString KAdapterName("Maanmittauslaitos");
const QString KFileNameFormat("%1/%2.png");

MaanmittauslaitosMapAdapter::MaanmittauslaitosMapAdapter(QObject* parent) :
    IMapAdapter(parent, tr("Tile folder info file (*.tiles)"))
{
    init();
    setDataFolder(findDefaultDataSource());
}

MaanmittauslaitosMapAdapter::~MaanmittauslaitosMapAdapter()
{
    pj_free(mProjEtrstm35Fin);
    pj_free(mProjWgs84);
}

MaanmittauslaitosMapAdapter::MaanmittauslaitosMapAdapter(QObject* parent, QString fileFilter, bool isAsync) :
    IMapAdapter(parent, fileFilter, isAsync)
{
    init();
}

void MaanmittauslaitosMapAdapter::init()
{
    mProjWgs84 = pj_init_plus("+proj=latlong +datum=WGS84 +ellps=WGS84");
    mProjEtrstm35Fin = pj_init_plus("+proj=utm +zone=35 +ellps=GRS80 +units=m +no_defs");

    // Default tile size.
    mTileSize = QSize(480, 480);
}

QString MaanmittauslaitosMapAdapter::name()
{
    return KAdapterName;
}

IMapAdapter::TDataSourceType MaanmittauslaitosMapAdapter::DataSourceType()
{
    return File;
}

bool MaanmittauslaitosMapAdapter::openDataSource(QString& fileName)
{
    qDebug() << fileName;

    if(!fileName.isEmpty())
    {
        mTilesFile = fileName;
        mTileSize = getTileSize(mTilesFile);
        QString folder = getXmlParam(mTilesFile, "AbsolutePath");
        if(folder.isEmpty())
        {
            QFileInfo fileInfo(mTilesFile);
            folder = fileInfo.absolutePath();
        }

        setDataFolder(folder);

        mSourceMapScale = qMax(getXmlParam(mTilesFile, "SourceMapScale").toInt(), 1);
        mTileScale = getXmlParam(mTilesFile, "Scale").toInt();

        emit openCompleted();

        return true;
    }

    return false;
}

QString MaanmittauslaitosMapAdapter::dataFolder()
{
    return mTilesFile;
}


QPoint MaanmittauslaitosMapAdapter::LatLonToDisplayCoordinate(const QPointF& coordinate)
{
    QPoint result;

    double x = KKJCoordinates::DegreeToRadian(coordinate.x());
    double y = KKJCoordinates::DegreeToRadian(coordinate.y());

    int res = pj_transform(mProjWgs84, mProjEtrstm35Fin, 1, 1, &x, &y, NULL);

    if(res == 0)
    {
        // Convert to local pixel coordinate.
        // First bias to origo.
        const QPoint KWorldOrigo(20006, 7817994);           // New
        result = QPoint(QPoint(x, y) - KWorldOrigo);
        result.setY(result.y() * -1);

        // Convert to tile.
        const qreal fPixelX = (qreal) (result.x() / mSourceMapScale) / (1200 * mTileScale);
        const qreal fPixelY = (qreal) (result.y() / mSourceMapScale) / (1200 * mTileScale);

        // Convert to local pixel
        result = QPoint(fPixelX * mTileSize.width() - mTileSize.width(), fPixelY * mTileSize.height() - mTileSize.height());    // TODO: fix that offset.
    }

    return result;
}

QPointF MaanmittauslaitosMapAdapter::DisplayCoordinateToLatLon(const QPoint& coordinate)
{
    // TODO:
    return QPointF();
}

QPixmap MaanmittauslaitosMapAdapter::loadTile(const QPoint& tile)
{
    const QString filePath(mDataFolder + KFileNameFormat.arg(tile.x()).arg(tile.y()));

    QPixmap pixmap;
    pixmap.load(filePath);
    return pixmap;
}

QPoint MaanmittauslaitosMapAdapter::defaultLocation() const
{
    const QString ptLeft = getXmlParam(mTilesFile, "Left");
    const QString ptTop = getXmlParam(mTilesFile, "Top");
    const QString ptRight= getXmlParam(mTilesFile, "Right");
    const QString ptBottom = getXmlParam(mTilesFile, "Bottom");

    const QRect mapArea(QPoint(ptLeft.toInt(), ptTop.toInt()), QPoint(ptRight.toInt(), ptBottom.toInt()));

    return mapArea.center();
}

QString MaanmittauslaitosMapAdapter::findDefaultDataSource()
{
    const QString folder = Utility::findDataFolder("MML");

    QDir dir(folder);

    QStringList filters;
    filters.append("*.tiles");
    dir.setNameFilters(filters);
    QStringList entryList = dir.entryList();
    if(!entryList.isEmpty())
    {
        qDebug() << entryList.first();
        mTilesFile = entryList.first();
        return folder;
    }

    return QString();
}

QString MaanmittauslaitosMapAdapter::getXmlParam(const QString& fileName, const QString& paramName)
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

QSize MaanmittauslaitosMapAdapter::getTileSize(const QString& fileName)
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
