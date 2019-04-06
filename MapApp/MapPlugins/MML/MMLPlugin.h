#ifndef MMLPLUGIN_H
#define MMLPLUGIN_H

#include <QObject>
#include <QtPlugin>

#include <proj_api.h>

#include "MapPluginInterface.h"
#include "importer.h"

class MMLPlugin : public QObject, public IMapPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.TrailMap.TrailMap.MapPluginInterface")
    Q_INTERFACES(IMapPluginInterface)

public:
    MMLPlugin();
    virtual ~MMLPlugin();

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

signals:
    void progress(int);
    void done();

public slots:
    void importProgress(int);
    void importDone();

protected:
    QSize getTileSize(const QString& fileName) const;
    QPoint defaultLocation(const QString& tilesFile) const;
    QString getXmlParam(const QString& fileName, const QString& paramName) const;
    QString mapLevel() const;
    QString findDefaultFile() const;
    void readTilesFile(const QString& fileName);
    void scanMapLevels();

private:
    QString mWorkingFolder;
    int mZoomLevel;
    QStringList mMapLevelsList;

    QPoint mDefaultLocation;
    QSize mTileSize;
    int mTileScale;
    int mSourceMapScale;

    projPJ mProjWgs84;
    projPJ mProjEtrstm35Fin;

    Importer* mImporter;
};

#endif // MMLPLUGIN_H
