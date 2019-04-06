#ifndef OPENSTREETMAPCOMPATIBLE_H
#define OPENSTREETMAPCOMPATIBLE_H

#include <QObject>
#include <QtPlugin>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimerEvent>
#include <QDateTime>
#include <QUrl>


#include "MapPluginInterface.h"

/**
  * Legacy format plugin
  *
  */
class OpenStreetMapCompatiblePlugin : public QObject, public IMapPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.TrailMap.TrailMap.MapPluginInterface")
    Q_INTERFACES(IMapPluginInterface)

public:
    OpenStreetMapCompatiblePlugin();
    virtual ~OpenStreetMapCompatiblePlugin();

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

public slots:
    void tileDownloaded();
    void delayOver();
    void openURL();
    void openHost();

signals:
    void dataChanged();
    void invalidate();

protected:
    void timerEvent(QTimerEvent* event);

protected:
    QPointF latLonToTile(const QPointF& latLon) const;
    QPointF tileToLatLon(const QPointF& tile) const;
    QString getTilePath(const QPoint& tile) const;
    QUrl getTileUrl(const QPoint& tile) const;
    void downloadTile(const QPoint& tile);
    void startDownload(const QString& url);
    QStringList scanProviders(const QString& folder) const;
    QString fullTilePath(const QString& folder) const;
    QString tileBasePath(const QString& fullTilePath) const;

protected:
    QString mHost;
    QString mFilenameFormat;
    int mZoom;

private:
    QString mWorkingFolder;
    QSize mTileSize;

    QStringList mDownloadQueue;
    QNetworkAccessManager* mNetworkAccessManager;
    QNetworkReply* mNetworkReply;
    int mTimerId;
    QDateTime mDownloadStartTime;
    QDateTime mLastErrorTime;
};

#endif // OPENSTREETMAPCOMPATIBLE_H
