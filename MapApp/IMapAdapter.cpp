#include "imapadapter.h"

#include <QDir>
#include <QDebug>

IMapAdapter::IMapAdapter(QObject* parent, QString fileFilter, bool isAsync) :
    QObject(parent),
    mFileFilter(fileFilter),
    mIsAsync(isAsync),
    mZoom(1),
    mMinZoom(1),
    mMaxZoom(1)
{
}

IMapAdapter::IMapAdapter(QObject* parent, bool isAsync) :
    QObject(parent),
    mIsAsync(isAsync),
    mZoom(1),
    mMinZoom(1),
    mMaxZoom(1)
{
}

IMapAdapter::IMapAdapter(QObject* parent, QString fileFilter, QString name, bool isAsync,
                         TDataSourceType dataSourceType, int minZoom, int maxZoom) :
    QObject(parent),
    mFileFilter(fileFilter),
    mIsAsync(isAsync),
    mAdapterName(name),
    mDataSourceType(dataSourceType),
    mMinZoom(minZoom),
    mMaxZoom(maxZoom)
{
    mZoom = validateZoom(mZoom);
}

QString IMapAdapter::name()
{
    return mAdapterName;
}

QString IMapAdapter::fileFilter() const
{
    return mFileFilter;
}

QString IMapAdapter::dataFolder()
{
    return mDataFolder;
}

int IMapAdapter::minZoom() const
{
    return mMinZoom;
}

int IMapAdapter::maxZoom() const
{
    return mMaxZoom;
}

int IMapAdapter::zoom() const
{
    return mZoom;
}

void IMapAdapter::setZoom(int zoom)
{
    mZoom = validateZoom(zoom);
}

IMapAdapter::TDataSourceType IMapAdapter::DataSourceType()
{
    return mDataSourceType;
}

void IMapAdapter::setDataFolder(const QString& path)
{
    if(!path.isEmpty())
    {
        mDataFolder = path;
        if(mDataFolder.right(1) != "/")
        {
            mDataFolder += "/";
        }

        qDebug() << "Data folder:" << mDataFolder;
    }
}

bool IMapAdapter::isAsync() const
{
    return mIsAsync;
}

bool IMapAdapter::containsData()
{
    // TODO:
    return true;
}

QSize IMapAdapter::tileSize() const
{
    return mTileSize;
}

// Returns a tile location
QPoint IMapAdapter::defaultLocation() const
{
    // Find a tile and check it's coordinate.
    // TODO:
/*
    QDir dir(mDataFolder);

    QFileInfoList fil = dir.entryInfoList();

    foreach(QFileInfo fi, fil)
    {
        QPoint tile;
        if(parseFileName(fi.fileName(), tile))
        {
            qDebug() << "Found tile: " << fi.fileName();
            return tile;
        }
    }
*/
    return QPoint();
}


bool IMapAdapter::parseFileName(const QString& fileName, QPoint& tile) const
{
    int x;
    int y;

    const char* str = fileName.toLocal8Bit().constData();
    bool success = sscanf(str, "tile%dx%d.png", &x, &y) == 2;

    tile.setX(x * mTileSize.width());
    tile.setY(y * mTileSize.height());

    return success;
}

int IMapAdapter::validateZoom(int zoom) const
{
    zoom = qMax(zoom, mMinZoom);
    zoom = qMin(zoom, mMaxZoom);
    return zoom;
}

void IMapAdapter::unload()
{
}

void IMapAdapter::setFileFilter(const QString& fileFilter)
{
    mFileFilter = fileFilter;
}

void IMapAdapter::setCopyrightNotice(const QString& text)
{
    mCopyrightNotice = text;
}

const QString& IMapAdapter::copyrightNotice() const
{
    return mCopyrightNotice;
}
