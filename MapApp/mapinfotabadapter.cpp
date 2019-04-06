#include "mapinfotabadapter.h"

#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QDebug>

#include "tab/Tfw.h"

const QString KAdapterName("MML World File");
const int KBitmapScale = 2;

MapInfoTabAdapter::MapInfoTabAdapter(QObject *parent) :
    MaanmittauslaitosMapAdapter(parent, tr("World Files (*.tfw)"), true),
    mTiffSplit(NULL),
    mThread(NULL)
{
}

QString MapInfoTabAdapter::name()
{
    return KAdapterName;
}

bool MapInfoTabAdapter::openDataSource(QString& path)
{
    qDebug() << "openDataSource:" << path;

    QFileInfo fileInfo(path);
    const QString fileName = fileInfo.baseName();

    const QString dataFolder = QDir::tempPath() + "/" + fileName;

    setDataFolder(dataFolder);

    // SEE: http://qt-project.org/doc/qt-4.7/qthread.html

    Tfw worldFile(path);
    const QString imagePath = fileInfo.absolutePath() + "/" +  worldFile.imageFileName();
    QImageReader image(imagePath);

    const qreal refScale = 2.5;     // 1:50k m/pixel
    const qreal sourceMapScale = worldFile.pixelSize().width() / refScale;

    const QRectF coordinateRect(worldFile.baseCoordinate(), QSizeF(image.size().width() * worldFile.pixelSize().width(), image.size().height() * worldFile.pixelSize().height()));
    mTiffSplit = new TiffSplit(imagePath, tileSize(), IMapAdapter::dataFolder(), KBitmapScale, coordinateRect, sourceMapScale);

    connect(mTiffSplit, SIGNAL(progress(int)), SLOT(tiffSplitProgress(int)));
    connect(mTiffSplit, SIGNAL(done()), SLOT(tiffSplitDone()));

    mThread = new QThread;
    connect(mThread, SIGNAL(finished()), this, SLOT(finished()));
    mTiffSplit->moveToThread(mThread);
    mThread->start();
    QMetaObject::invokeMethod(mTiffSplit, "split", Qt::QueuedConnection);

    return true;
}

void MapInfoTabAdapter::tiffSplitProgress(int value)
{
    emit progress(value);
}

void MapInfoTabAdapter::tiffSplitDone()
{
    mThread->exit();
}

void MapInfoTabAdapter::finished()
{
    QString tilesFile = mTiffSplit->infoFile();
    MaanmittauslaitosMapAdapter::openDataSource(tilesFile);
    //emit openCompleted();

    delete mThread;
    mThread = NULL;

    delete mTiffSplit;
    mTiffSplit = NULL;
}

void MapInfoTabAdapter::unload()
{
    // TODO: cancel thread

    // Remove temp files
    rmDir(dataFolder());
}

bool MapInfoTabAdapter::rmDir(const QString& path) const
{
    bool result = true;
    QDir dir(path);

    if(dir.exists(path))
    {
        Q_FOREACH(QFileInfo fileInfo, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  |
                                                        QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if(fileInfo.isDir())
            {
                result = rmDir(fileInfo.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(fileInfo.absoluteFilePath());
            }

            if(!result)
            {
                return result;
            }
        }
        result = dir.rmdir(path);
    }

    return result;
}
