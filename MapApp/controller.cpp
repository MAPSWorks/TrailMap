#include "controller.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QList>
#include <QtXml/QXmlInputSource>
#include <QXmlStreamWriter>
#include <QDir>

#include "Config.h"
#include "utility.h"
#include "GpxParser.h"
#include "LmxParser.h"
#include "GpxWriter.h"

#include "karttapaikkamapadapter.h"
//#include "maanmittauslaitosmapadapter.h"
//#include "mapinfotabadapter.h"
//#include "OpenStreetMapCompatibleAdapter.h"
//#include "OpenStreetMapAdapter.h"

#include "MapPluginInterface.h"


const QString KFileBookmarks("bookmarks.txt");

Controller::Controller(QObject *parent) :
    QObject(parent),
    mDocument(NULL),
    mPluginAdapter(NULL)
{
    // Default data folder.
    mDataFolder = Utility::findWorkingFolder();     // TODO: remove this member.

    findMapPlugins("../lib");       // Android
    findMapPlugins("plugins");
    qDebug () << "Found" << mPluginList.count() << "plugins.";

    connect(&mLocationSource, SIGNAL(newLocation(QPointF)), this, SLOT(newLocation(QPointF)));
}

Controller::~Controller()
{
    foreach(IMapAdapter* adapter, mAdapters)
    {
        adapter->unload();
        delete adapter;
    }
}

const QStringList Controller::adapterList() const
{
    QStringList adapterList;

    foreach(QString plugin, mPluginList)
    {
        const QFileInfo fi(plugin);
        adapterList.append(fi.baseName());
    }

    return adapterList;
}

IMapAdapter* Controller::adapterByName(const QString& adapterName)
{
    foreach(QString pluginFile, mPluginList)
    {
        const QFileInfo fi(pluginFile);
        if(fi.baseName() == adapterName)
        {
            delete mPluginAdapter;
            mPluginAdapter = openPlugin(pluginFile);

            QString pluginDataFolder(mDataFolder + adapterName);
            QDir makedir;
            makedir.mkpath(pluginDataFolder);
            //mPluginAdapter->openDataSource(pluginDataFolder);   // TODO: this gets called twice.
            return mPluginAdapter;
            // TODO: handle new plugins too!
        }
    }

    // No adapters available!
    qDebug() << "Adapter not found: " << adapterName;
    return NULL;
}

IMapAdapter* Controller::currentAdapter()
{
    return mPluginAdapter;
}

void Controller::setAdapter(const QString& adapterName)
{

    // TODO: find adapter with the given name.
    //mAdapter = new KarttapaikkaMapAdapter();

    //mDataFolder = mAdapter
            //Utility::findDataFolder(KProvider);


    // Read bookmarks
    /*
    // TODO:
    QFile file;
    file.setFileName(mAdapter->dataFolder() + KFileBookmarks);
    if(file.exists())
    {
        readBookmarksFromTxt(file.fileName());
    }
    */
    // TODO: check GPX bookmarks file.
}

void Controller::setupAdapterMenu(QMenu& menu)
{
    mPluginAdapter->plugin().createMenu(menu);
}

QList<Poi>& Controller::bookmarks()
{
    return mListBookmarks;
}

const QString& Controller::dataFolder() const
{
    return mDataFolder;
}

QList<Poi> Controller::OpenFile(const QString& fileName)
{
    const QFileInfo fi(fileName);
    const QString ext = fi.suffix();

    if(ext == "gpx")
    {
        return GpxParser::Parse(fileName);
    }
    else if(ext == "lmx")
    {
        return LmxParser::Parse(fileName);
    }
    else
    {
        return QList<Poi>();
    }
}

void Controller::newDocument()
{
    delete mDocument;
    mDocument = new Document();
}

void Controller::OpenDocument(const QString& fileName)
{
    if(fileName.isEmpty())
    {
        return;
    }

    // TODO: for now we support only one document at a time.
    // TODO: ask to save
    delete mDocument;
    mDocument = NULL;

    const QFileInfo fi(fileName);
    const QString ext = fi.suffix().toLower();

    if(ext == "gpx")
    {
        mDocument = GpxParser::ParseDocument(fileName);
    }
    else if(ext == "lmx")
    {
        mDocument = LmxParser::ParseDocument(fileName);
    }
    else
    {
        //return QList<Poi>();
    }
}

void Controller::findMapPlugins(const QString& path)
{
    qDebug () << "Searching for plugins in path:" << path;

    /*
      SEE:
      http://qt-project.org/doc/qt-4.8/plugins-howto.html
      http://michael-stengel.com/blog/?p=4
      */

    QDir pluginsDir(path);

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        PluginAdapter* plugin = openPlugin(pluginsDir.absoluteFilePath(fileName));
        if(plugin)
        {
            delete plugin;
            qDebug () << "Found:" << fileName;
            mPluginList.append(pluginsDir.absolutePath() + "/" + fileName);
        }
    }
}

PluginAdapter* Controller::openPlugin(const QString& fileName)
{
    QPluginLoader pluginLoader(fileName);
    QObject* object = pluginLoader.instance();

    IMapPluginInterface* plugin = qobject_cast<IMapPluginInterface*>(object);

    if(plugin)
    {
        PluginAdapter* pluginAdapter = new PluginAdapter(*plugin, fileName, this);
        connect(object, SIGNAL(progress(int)), pluginAdapter, SLOT(importProgress(int)));
        connect(object, SIGNAL(done()), pluginAdapter, SLOT(importCompleted()));
        connect(object, SIGNAL(dataChanged()), pluginAdapter, SLOT(mapDataChanged()));
        connect(object, SIGNAL(invalidate()), pluginAdapter, SLOT(mapDataInvalidated()));

        qDebug () << "Plugin opened:" << fileName;
        return pluginAdapter;
    }

    return NULL;
}

void Controller::newLocation(QPointF location)
{
    emit locationChanged(location);

    // TODO: apply filter to location.
    mListRoutePoints.append(location);
}

QPointF Controller::location() const
{
    return mLocationSource.location();
}

const QList<QPointF>& Controller::routePoints() const
{
    return mListRoutePoints;
}

IPoiSource* Controller::poiSource() const
{
    return mDocument;
}

IRouteSource* Controller::routeSource() const
{
    return mDocument;
}

Document* Controller::document() const
{
    return mDocument;
}

void Controller::save(Document& document, const QString& filename)
{
    // See: http://www.developer.nokia.com/Community/Wiki/Generate_XML_programatically_in_Qt

    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter* xmlWriter = new QXmlStreamWriter();

        xmlWriter->setDevice(&file);

        xmlWriter->writeStartDocument();
        xmlWriter->writeStartElement("gpx");

        foreach(Poi poi, document.poiList())
        {
            // wpt element
            xmlWriter->writeStartElement("wpt");

            xmlWriter->writeAttribute("lat", QString::number(poi.latLon().y(), 'g', 10));   // todo: checxk precision
            xmlWriter->writeAttribute("lon", QString::number(poi.latLon().x(), 'g', 10));   // todo: checxk precision

            // name element
            xmlWriter->writeStartElement("name");
            xmlWriter->writeCharacters(poi.title());
            xmlWriter->writeEndElement();

            // cmt element
            xmlWriter->writeStartElement("cmt");
            xmlWriter->writeEndElement();

            xmlWriter->writeEndElement();
        }

        xmlWriter->writeEndElement();

        xmlWriter->writeEndDocument();
        delete xmlWriter;
    }
    else
    {
        // TODO: handle error.
    }
}

void Controller::loadBookmarks(const QString& folder)
{
    //readBookmarksFromTxt(folder + QDir::separator() + "bookmarks.txt");
    mBookmarkFile = folder + QDir::separator() + "bookmarks.gpx";
    readBookmarksFromGpx(mBookmarkFile);
}

void Controller::readBookmarksFromGpx(QString filename)
{
    mListBookmarks = GpxParser::Parse(filename);
}

void Controller::saveBookmarks()
{
    if(!mBookmarkFile.isEmpty())
    {
        GpxWriter writer(mBookmarkFile);

        foreach(Poi bookmark, mListBookmarks)
        {
            writer.addWpt(bookmark.latLon(), bookmark.title());
        }
    }
}
