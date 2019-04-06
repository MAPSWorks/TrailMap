#include "OpenStreetMapCompatiblePlugin.h"

#include <QDir>
#include <QFile>
#include <QMath.h>
#include <QInputDialog>
#include <QNetworkRequest>

#include <QDebug>

const QString KFileNameFormat("%1/%2/%3.png");
const QPointF KDefaultCoordinate(0.0, 51.0);
const int DOWNLOAD_TIMEOUT_ERROR = 1*60*1000;
#define USER_AGENT "TrailMap/0.0-beta"

// TODO: move these.
static qreal DegreeToRadian(const qreal aDegrees)
{
    return aDegrees * M_PI / 180.0;
}

static qreal RadianToDegree(const qreal aRadians)
{
    return aRadians * 180.0 / M_PI;
}

OpenStreetMapCompatiblePlugin::OpenStreetMapCompatiblePlugin() :
    QObject(),
    mZoom(minZoom()),
    mNetworkReply(NULL)
{
    mFilenameFormat = KFileNameFormat;

    // Default tile size.
    mTileSize = QSize(256, 256);

    mNetworkAccessManager = new QNetworkAccessManager(this);
    mLastErrorTime = QDateTime::currentDateTime();
    mLastErrorTime = mLastErrorTime.addMSecs(-DOWNLOAD_TIMEOUT_ERROR);

    mHost = "http://otile1.mqcdn.com/tiles/1.0.0/osm";
}

OpenStreetMapCompatiblePlugin::~OpenStreetMapCompatiblePlugin()
{
}

void OpenStreetMapCompatiblePlugin::setWorkingFolder(const QString& path)
{
    mWorkingFolder = path;
}

void OpenStreetMapCompatiblePlugin::openDataSource(QUrl&)
{

}

void OpenStreetMapCompatiblePlugin::importData(QUrl&)
{

}

void OpenStreetMapCompatiblePlugin::createMenu(QMenu& menu)
{
    QAction* actionOpenUrl = menu.addAction(tr("Open URL..."));
    connect(actionOpenUrl, SIGNAL(triggered()), this, SLOT(openURL()));

    menu.addSeparator();

    QStringList hosts = scanProviders(mWorkingFolder);
    foreach(QString host, hosts)
    {
        QAction* action = menu.addAction(host);
        action->setData(QVariant(host));
        connect(action, SIGNAL(triggered()), this, SLOT(openHost()));
    }
}

QPoint OpenStreetMapCompatiblePlugin::LatLonToDisplayCoordinate(const QPointF& coordinate) const
{
    const QPointF tile = latLonToTile(coordinate);

    QPoint pixels(tile.x() * mTileSize.width(), tile.y() * mTileSize.height());

    return pixels;
}

QPointF OpenStreetMapCompatiblePlugin::DisplayCoordinateToLatLon(const QPoint& coordinate) const
{
    const QPointF tile(coordinate.x() / (qreal) mTileSize.width(), coordinate.y() / (qreal) mTileSize.height());
    return tileToLatLon(tile);
}

QSize OpenStreetMapCompatiblePlugin::tileSize() const
{
    return mTileSize;
}

QPixmap OpenStreetMapCompatiblePlugin::loadTile(const QPoint& tile)
{
    const QString filePath = getTilePath(tile);

    QPixmap pixmap;
    pixmap.load(filePath);

    if(pixmap.isNull())
    {
        // Try to load as JPG.
        pixmap.load(filePath, "jpg");
    }

    if(pixmap.isNull())
    {
        downloadTile(tile);
    }

    return pixmap;
}

QPoint OpenStreetMapCompatiblePlugin::defaultLocation() const
{
    const QPointF tile = latLonToTile(KDefaultCoordinate);
    qDebug() << "Default tile location:" << tile.x() << tile.y() << zoom();
    return QPoint(tile.x(), tile.y());
}

int OpenStreetMapCompatiblePlugin::minZoom() const
{
    return 6;
}

int OpenStreetMapCompatiblePlugin::maxZoom() const
{
    return 15;
}

int OpenStreetMapCompatiblePlugin::zoom() const
{
    return mZoom;
}

void OpenStreetMapCompatiblePlugin::setZoom(int zoom)
{
    mZoom = zoom;
    mDownloadQueue.clear();
}

QString OpenStreetMapCompatiblePlugin::getTilePath(const QPoint& tile) const
{
    const QUrl url = getTileUrl(tile);
    return QString(mWorkingFolder + url.host() + url.path());
}

QUrl OpenStreetMapCompatiblePlugin::getTileUrl(const QPoint& tile) const
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

void OpenStreetMapCompatiblePlugin::downloadTile(const QPoint& tile)
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

void OpenStreetMapCompatiblePlugin::startDownload(const QString& url)
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

void OpenStreetMapCompatiblePlugin::tileDownloaded()
{
    mNetworkReply->disconnect();

    // Remove this tile from the queue.
    const QUrl url = mNetworkReply->url();
    mDownloadQueue.removeOne(url.toString());

    if(mNetworkReply->error() == QNetworkReply::NoError)
    {
        // Write tile to file.
        const QString filePath = mWorkingFolder + url.host() + url.path();
        qDebug() << "Save tile to:" << filePath;

        QDir dir(filePath);
        const QString mkDir(filePath.left(filePath.lastIndexOf('/')));
        dir.mkpath(mkDir);

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);
        file.write(mNetworkReply->readAll());
        file.close();

        // Repaint map
        // TODO:
        emit dataChanged();
    }
    else
    {
        qDebug() << "Download error.";
        mLastErrorTime = QDateTime::currentDateTime();
    }

    mNetworkReply->close();

    // Calculate delay for bandwidth throttling (delay == last download time).
    const int duration = mDownloadStartTime.msecsTo(QDateTime::currentDateTime());
    mTimerId = startTimer(qMax(duration, 10));
    qDebug() << "Download took:" << duration << "ms";
}

void OpenStreetMapCompatiblePlugin::delayOver()
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
    else
    {
        qDebug() << "DOWNLOAD COMPLETE.";
    }
}

void OpenStreetMapCompatiblePlugin::openURL()
{
    bool ok = false;
    QString url = QInputDialog::getText(NULL, "Type URL:", "Label", QLineEdit::Normal, "", &ok);
    if(ok && !url.isEmpty())
    {
        qDebug() << "Use URL:" << url;

        mHost = url;
        // TODO: scan providers
        // TODO: update menu
        // TODO: render map

        emit invalidate();
        emit dataChanged();
    }
}

void OpenStreetMapCompatiblePlugin::openHost()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action)
    {
        const QVariant data = action->data();
        const QString host = data.toString();

        qDebug() << "Switch host:" << host;

        mDownloadQueue.clear();

        mHost = host;

        emit invalidate();
        emit dataChanged();
    }
}

void OpenStreetMapCompatiblePlugin::timerEvent(QTimerEvent* event)
{
    if(event->timerId() == mTimerId)
    {
        delayOver();
    }
}

QPointF OpenStreetMapCompatiblePlugin::latLonToTile(const QPointF& latLon) const
{
    const qreal lat_rad = DegreeToRadian(latLon.y());
    const int n = qPow(2, zoom());

    const qreal tileX = n * ((latLon.x() + 180) / 360);
    const qreal tileY = n * (1 - (qLn(qTan(lat_rad) + 1 / qCos(lat_rad)) / M_PI)) / 2;

    return QPointF(tileX, tileY);
}

QPointF OpenStreetMapCompatiblePlugin::tileToLatLon(const QPointF& tile) const
{
    const int n = qPow(2, zoom());
    const qreal lon_deg = tile.x() / n * 360.0 - 180.0;
    const qreal lat_rad = qAtan(sinh(M_PI * (1 - 2 * tile.y() / n)));

    return QPointF(lon_deg, RadianToDegree(lat_rad));
}

QStringList OpenStreetMapCompatiblePlugin::scanProviders(const QString& folder) const
{
    Q_ASSERT(folder.right(1) == "/");
    QDir dir(folder);

    QStringList hostList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);

    QStringList results;

    foreach(QString host, hostList)
    {
        const QString recurseFolder(folder + host);
        QString hostPath = fullTilePath(recurseFolder);
        if(!hostPath.isEmpty())
        {
            qDebug() << "Found cached host folder:" << hostPath;
            QString provider = tileBasePath(hostPath);
            provider = provider.right(provider.length() - folder.length());
            QUrl url("http://" + provider); // TODO: problem with the setScheme() method.
            if(url.isValid())
            {
                results.append(url.toString());
            }
        }
    }

    return results;
}

QString OpenStreetMapCompatiblePlugin::fullTilePath(const QString& folder) const
{
    QDir dir(folder);
    QStringList entries = dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::NoSort);

    QString path(folder + "/" + entries.first());
    if(!path.endsWith(".png"))
    {
        path = fullTilePath(path);
    }

    return path;
}

// http://c.tile.opencyclemap.org/cycle/11/1100/671.png
// C:\tiledata\OsmCompatibled\c.tile.opencyclemap.org\cycle\6\31\20.png
// const QString KFileNameFormat("%1/%2/%3.png");
QString OpenStreetMapCompatiblePlugin::tileBasePath(const QString& fullTilePath) const
{
    // TODO: review this.

    QString result(fullTilePath);

    const int count = KFileNameFormat.split("/", QString::SkipEmptyParts).count();
    for(int i = 0; i < count; i++)
    {
        int ix = result.lastIndexOf("/");
        if(ix > -1)
        {
            result = result.left(ix);
        }
        else
        {
            break;
        }
    }

    return result;
}


