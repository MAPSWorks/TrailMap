#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QPluginLoader>

#include "imapadapter.h"
#include "bookmark.h"
#include "poi.h"
#include "LocationSource.h"
#include "document.h"
#include "MapPluginInterface.h"
#include "pluginadapter.h"
#include "GpxWriter.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

public:
    const QStringList adapterList() const;
    IMapAdapter* adapterByName(const QString& adapterName);
    IMapAdapter* currentAdapter();
    void setAdapter(const QString& adapterName);
    void setupAdapterMenu(QMenu&);

    QList<Poi>& bookmarks();    // TODO: move to adapter. ?
    const QString& dataFolder() const;
    QList<Poi> OpenFile(const QString& fileName);
    void newDocument();
    void OpenDocument(const QString& fileName);
    QPointF location() const;
    const QList<QPointF>& routePoints() const;
    IPoiSource* poiSource() const;
    IRouteSource* routeSource() const;
    Document* document() const;
    void save(Document& document, const QString& filename);
    void loadBookmarks(const QString& folder);
    void saveBookmarks();

signals:
    void locationChanged(QPointF location);
    
public slots:
    void newLocation(QPointF);
    
private:
    void findMapPlugins(const QString& path);
    PluginAdapter* openPlugin(const QString& name);

    //QList<Poi> OpenGpx(const QString& fileName);
    void readBookmarksFromGpx(QString filename);

private:
    //IMapAdapter* mAdapter;
    QList<IMapAdapter*> mAdapters;
    QString mDataFolder;
    QList<Poi> mListBookmarks;
    QString mBookmarkFile;
    QList<QPointF> mListRoutePoints;

    LocationSource mLocationSource;

    Document* mDocument;

    QStringList mPluginList;
    PluginAdapter* mPluginAdapter;
};

#endif // CONTROLLER_H
