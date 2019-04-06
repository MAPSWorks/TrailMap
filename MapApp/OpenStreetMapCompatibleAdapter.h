#ifndef OPENSTREETMAPCOMPATIBLEADAPTER_H
#define OPENSTREETMAPCOMPATIBLEADAPTER_H

#include "IMapAdapter.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimerEvent>
#include <QDateTime>
#include <QUrl>

class OpenStreetMapCompatibleAdapter : public IMapAdapter
{
    Q_OBJECT

public:
    OpenStreetMapCompatibleAdapter(QObject* parent = 0);
    OpenStreetMapCompatibleAdapter(QObject* parent, const QString& adapterName, int maxZoom, int minZoom,
                                   IMapAdapter::TDataSourceType dataSourceType, const QString& fileNameFormat);

public: // Inherited abstract methods
    QString dataFolder();
    void setDataFolder(const QString& path);
    virtual bool openDataSource(QString& url);
    QPoint LatLonToDisplayCoordinate(const QPointF& coordinate);
    QPointF DisplayCoordinateToLatLon(const QPoint& coordinate);
    QPixmap loadTile(const QPoint& tile);
    QPoint defaultLocation() const;
    void setZoom(int zoom);

public slots:
    void tileDownloaded();
    void delayOver();

protected:
    void timerEvent(QTimerEvent* event);

protected:
    QString mHost;
    QString mFilenameFormat;

private:
    QPointF latLonToTile(const QPointF& latLon) const;
    QPointF tileToLatLon(const QPointF& tile) const;
    void downloadTile(const QPoint& tile);
    void startDownload(const QString& url);
    QString getTilePath(const QPoint& tile) const;
    QUrl getTileUrl(const QPoint& tile) const;

private:
    QStringList mDownloadQueue;
    QNetworkAccessManager* mNetworkAccessManager;
    QNetworkReply* mNetworkReply;
    int mTimerId;
    QDateTime mDownloadStartTime;
    QDateTime mLastErrorTime;
};

#endif // OPENSTREETMAPCOMPATIBLEADAPTER_H
