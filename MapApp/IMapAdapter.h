#ifndef MAPADAPTER_H
#define MAPADAPTER_H

#include <QObject>
#include <QPoint>
#include <QPointF>
#include <QString>
#include <QPixmap>

class IMapAdapter : public QObject
{
public:
    enum TDataSourceType {
        Directory,
        File,
        Net,
        None
    };

    Q_OBJECT

public:
    IMapAdapter(QObject* parent, QString fileFilter, bool isAsync = false);
    IMapAdapter(QObject* parent, bool isAsync = false);
    IMapAdapter(QObject* parent, QString fileFilter, QString name, bool isAsync,
                TDataSourceType dataSourceType, int minZoom = 1, int maxZoom = 1);

signals:
    void openCompleted();
    void progress(int progress);
    void dataChanged();
    void invalidate();

public:
    virtual QString name();
    virtual TDataSourceType DataSourceType();
    virtual QString fileFilter() const;
    bool isAsync() const;
    virtual bool openDataSource(QString& path) = 0;
    virtual QPoint LatLonToDisplayCoordinate(const QPointF& coordinate) = 0;
    virtual QPointF DisplayCoordinateToLatLon(const QPoint& coordinate) = 0;
    virtual QString dataFolder();
    virtual bool containsData();
    virtual void setDataFolder(const QString& path);
    virtual QPixmap loadTile(const QPoint& tile) = 0;
    virtual QSize tileSize() const;

    // Return value is tile.
    virtual QPoint defaultLocation() const;
    virtual void unload();
    virtual void setFileFilter(const QString& fileFilter);
    int minZoom() const;
    int maxZoom() const;
    virtual int zoom() const;
    virtual void setZoom(int zoom);
    const QString& copyrightNotice() const;
    // TODO: bookmarks

protected:
    void setCopyrightNotice(const QString& text);

private:
    bool parseFileName(const QString& fileName, QPoint& tile) const;
    int validateZoom(int zoom) const;

protected:
    QString mAdapterName;
    TDataSourceType mDataSourceType;
    int mMinZoom;
    int mMaxZoom;
    int mZoom;
    QString mDataFolder;
    bool mIsAsync;
    QSize mTileSize;
    QString mFileFilter;
    QString mCopyrightNotice;
};

#endif // MAPADAPTER_H
