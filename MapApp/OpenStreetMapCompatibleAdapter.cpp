#include "OpenStreetMapCompatibleAdapter.h"
#include "Coordinates.h"
#include "Utility.h"
#include "Config.h"

#include <QFileInfo>
#include <QDir>
#include <QNetworkRequest>
#include <QDebug>

const QString KFileNameFormat("%1/%2/%3.png");
const QPointF KDefaultCoordinate(0.0, 51.0);
const int DOWNLOAD_TIMEOUT_ERROR = 1*60*1000;

OpenStreetMapCompatibleAdapter::OpenStreetMapCompatibleAdapter(QObject* parent)
    : IMapAdapter(parent, "fileFilter", "OpenStreetMapCompatible", false,
                  IMapAdapter::Net, 6, 15),
    mNetworkReply(NULL)
{
    mFilenameFormat = KFileNameFormat;
    // Default tile size.
    mTileSize = QSize(256, 256);

    setDataFolder(Utility::findDataFolder(mAdapterName));

    mNetworkAccessManager = new QNetworkAccessManager(this);

    mLastErrorTime = QDateTime::currentDateTime();
    mLastErrorTime = mLastErrorTime.addMSecs(-DOWNLOAD_TIMEOUT_ERROR);
}

OpenStreetMapCompatibleAdapter::OpenStreetMapCompatibleAdapter(QObject* parent, const QString& adapterName, int maxZoom, int minZoom,
                                                               IMapAdapter::TDataSourceType dataSourceType, const QString& fileNameFormat)
    : IMapAdapter(parent, "", adapterName, false,
                  dataSourceType, minZoom, maxZoom),
    mNetworkReply(NULL)
{
    mFilenameFormat = fileNameFormat;
    // Default tile size.
    mTileSize = QSize(256, 256);

    setDataFolder(Utility::findDataFolder(mAdapterName));

    mNetworkAccessManager = new QNetworkAccessManager(this);

    mLastErrorTime = QDateTime::currentDateTime();
    mLastErrorTime = mLastErrorTime.addMSecs(-DOWNLOAD_TIMEOUT_ERROR);
}

QString OpenStreetMapCompatibleAdapter::dataFolder()
{
    return mDataFolder + "|" + mHost;
}

void OpenStreetMapCompatibleAdapter::setDataFolder(const QString& path)
{
    const QStringList list = path.split('|');

    if(list.count() == 2)
    {
        IMapAdapter::setDataFolder(list.first());
        mHost = list.last();

        qDebug() << "Host:" << mHost;
    }
    else
    {
        IMapAdapter::setDataFolder(path);
    }
}

bool OpenStreetMapCompatibleAdapter::openDataSource(QString& url)
{
    mHost = url;

    QString path = Utility::findDataFolder(mAdapterName);
    setDataFolder(path);

    emit openCompleted();

    return true;
}

QPoint OpenStreetMapCompatibleAdapter::LatLonToDisplayCoordinate(const QPointF& coordinate)
{
    const QPointF tile = latLonToTile(coordinate);

    QPoint pixels(tile.x() * mTileSize.width(), tile.y() * mTileSize.height());

    return pixels;
}

QPointF OpenStreetMapCompatibleAdapter::DisplayCoordinateToLatLon(const QPoint& coordinate)
{
    const QPointF tile(coordinate.x() / (qreal) mTileSize.width(), coordinate.y() / (qreal) mTileSize.height());
    return tileToLatLon(tile);
}

QPixmap OpenStreetMapCompatibleAdapter::loadTile(const QPoint& tile)
{
    const QString filePath = getTilePath(tile);

    QPixmap pixmap;
    pixmap.load(filePath);

    if(pixmap.isNull())
    {
        downloadTile(tile);
    }

    return pixmap;
}

QString OpenStreetMapCompatibleAdapter::getTilePath(const QPoint& tile) const
{
    const QUrl url = getTileUrl(tile);
    return QString(mDataFolder + url.host() + url.path());
}

QUrl OpenStreetMapCompatibleAdapter::getTileUrl(const QPoint& tile) const
{
    QUrl url(mHost);

    if(url.path().right(1) != "/")
    {
        url.setPath(url.path() + "/");
    }

    url.setPath(url.path() + mFilenameFormat.arg(zoom()).arg(tile.x()).arg(tile.y()));
    url.setScheme("http");
    return url;
}

void OpenStreetMapCompatibleAdapter::downloadTile(const QPoint& tile)
{
    // TODO: add tile validation here.

    if(mLastErrorTime.msecsTo(QDateTime::currentDateTime()) < DOWNLOAD_TIMEOUT_ERROR)
    {
        qDebug() << "Ignore download.";
        // Pause downloading on error.
        mDownloadQueue.clear();
        return;
    }

    QUrl host(mHost);
    const QUrl tileUrl(getTileUrl(tile));
    const QString url(tileUrl.toString());

    if(tileUrl.isValid())
    {
        if(!mDownloadQueue.contains(url))
        {
            mDownloadQueue.append(url);
        }

        if(mNetworkReply == NULL)
        {
            startDownload(url);
        }
    }
}

void OpenStreetMapCompatibleAdapter::startDownload(const QString& url)
{
    qDebug() << "Downloading URL:" << url;

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", USER_AGENT);
    mNetworkReply = mNetworkAccessManager->get(request);
    connect(mNetworkReply, SIGNAL(finished()), SLOT(tileDownloaded()));

    mDownloadStartTime = QDateTime::currentDateTime();

    // TODO: what if download fails or needs to be aborted ?
}

void OpenStreetMapCompatibleAdapter::tileDownloaded()
{
    mNetworkReply->disconnect();

    // Remove this tile from the queue.
    const QUrl url = mNetworkReply->url();
    mDownloadQueue.removeOne(url.toString());

    if(mNetworkReply->error() == QNetworkReply::NoError)
    {
        // Write tile to file.
        const QString filePath = mDataFolder + url.host() + url.path();
        //qDebug() << "Save tile to:" << filePath;

        QDir dir(filePath);
        const QString mkDir(filePath.left(filePath.lastIndexOf('/')));
        dir.mkpath(mkDir);

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);
        file.write(mNetworkReply->readAll());
        file.close();

        // Repaint map
        emit dataChanged();
    }
    else
    {
        mLastErrorTime = QDateTime::currentDateTime();
    }

    mNetworkReply->close();

    // Calculate delay for bandwidth throttling (delay == last download time).
    const int duration = mDownloadStartTime.msecsTo(QDateTime::currentDateTime());
    mTimerId = startTimer(qMax(duration, 10));
    qDebug() << "Download took:" << duration << "ms";
}

void OpenStreetMapCompatibleAdapter::delayOver()
{
    killTimer(mTimerId);

    mNetworkReply = NULL;

    // Download next tile.
    if(mDownloadQueue.count() > 0)
    {
        // Downloaded the most recently requested tile.
        const QString url = mDownloadQueue.last();
        startDownload(url);
    }
}

void OpenStreetMapCompatibleAdapter::timerEvent(QTimerEvent* event)
{
    if(event->timerId() == mTimerId)
    {
        delayOver();
    }
}

QPoint OpenStreetMapCompatibleAdapter::defaultLocation() const
{
    const QPointF tile = latLonToTile(KDefaultCoordinate);
    qDebug() << "Default tile location:" << tile.x() << tile.y() << zoom();
    return QPoint(tile.x(), tile.y());
}

void OpenStreetMapCompatibleAdapter::setZoom(int zoom)
{
    // Reset download queue on zoom.
    mDownloadQueue.clear();
    IMapAdapter::setZoom(zoom);
}

QPointF OpenStreetMapCompatibleAdapter::latLonToTile(const QPointF& latLon) const
{
    const qreal lat_rad = KKJCoordinates::DegreeToRadian(latLon.y());
    const int n = qPow(2, zoom());

    const qreal tileX = n * ((latLon.x() + 180) / 360);
    const qreal tileY = n * (1 - (qLn(qTan(lat_rad) + 1 / qCos(lat_rad)) / M_PI)) / 2;

    return QPointF(tileX, tileY);
}

QPointF OpenStreetMapCompatibleAdapter::tileToLatLon(const QPointF& tile) const
{
    const int n = qPow(2, zoom());
    const qreal lon_deg = tile.x() / n * 360.0 - 180.0;
    const qreal lat_rad = qAtan(sinh(M_PI * (1 - 2 * tile.y() / n)));

    return QPointF(lon_deg, KKJCoordinates::RadianToDegree(lat_rad));
}
