#ifndef MAANMITTAUSLAITOSMAPADAPTER_H
#define MAANMITTAUSLAITOSMAPADAPTER_H

#include "IMapAdapter.h"

#include <QObject>
#include <proj_api.h>

class MaanmittauslaitosMapAdapter : public IMapAdapter
{
public:
    MaanmittauslaitosMapAdapter(QObject* parent = 0);
    MaanmittauslaitosMapAdapter(QObject* parent, QString fileFilter, bool isAsync);
    virtual ~MaanmittauslaitosMapAdapter();

public:
    QString name();
    TDataSourceType DataSourceType();
    bool openDataSource(QString& path);
    QPoint LatLonToDisplayCoordinate(const QPointF& coordinate);
    QPointF DisplayCoordinateToLatLon(const QPoint& coordinate);
    QPixmap loadTile(const QPoint& tile);
    QPoint defaultLocation() const;

    QString dataFolder();

protected:
    void init();
    QString findDefaultDataSource();
    static QString getXmlParam(const QString& fileName, const QString& paramName);
    static QSize getTileSize(const QString& fileName);


protected:
    QString mTilesFile;

    projPJ mProjWgs84;
    projPJ mProjEtrstm35Fin;

    int mTileScale;
    int mSourceMapScale;
};

#endif // MAANMITTAUSLAITOSMAPADAPTER_H
