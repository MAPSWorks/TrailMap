#include "importer.h"

#include <QFileInfo>
#include <QThread>
#include <QFileDialog>
#include <QDebug>

#include "Tfw.h"

const int KBitmapScale = 2;

Importer::Importer(QString& destinationFolder, QSize& tileSize, QObject* parent) :
    mDestinationFolder(destinationFolder),
    mTileSize(tileSize),
    QObject(parent)
{
}

// TODO: this is a test function!
void Importer::import()
{
    const QString fileName = QFileDialog::getOpenFileName(NULL, tr("Import map data"), "", tr("MML TFW Files (*.tfw)"));

    if(!fileName.isEmpty())
    {
        import(fileName);
    }
}

void Importer::import(QString fileName)
{
    qDebug() << "Importing" << fileName << "to folder" << mDestinationFolder;

    emit progress(0);

    QFileInfo fileInfo(fileName);
    //const QString fileName = fileInfo.baseName();

    //const QString dataFolder = QDir::tempPath() + "/" + fileName;

    //setDataFolder(dataFolder);

    // SEE: http://qt-project.org/doc/qt-4.7/qthread.html

    Tfw worldFile(fileInfo.absoluteFilePath());
    const QString imagePath = fileInfo.absolutePath() + "/" +  worldFile.imageFileName();
    QImageReader image(imagePath);

    const qreal refScale = 2.5;     // m/pixel (1:50k)
    const qreal sourceMapScale = worldFile.pixelSize().width() / refScale;

    const QRectF coordinateRect(worldFile.baseCoordinate(), QSizeF(image.size().width() * worldFile.pixelSize().width(), image.size().height() * worldFile.pixelSize().height()));
    mTiffSplit = new TiffSplit(imagePath, mTileSize, mDestinationFolder + QString::number(sourceMapScale) + "/", KBitmapScale, coordinateRect, sourceMapScale);

    connect(mTiffSplit, SIGNAL(progress(int)), SLOT(tiffSplitProgress(int)));
    connect(mTiffSplit, SIGNAL(done()), SLOT(tiffSplitDone()));

    mThread = new QThread;
    connect(mThread, SIGNAL(finished()), this, SLOT(finished()));       // TODO:
    mTiffSplit->moveToThread(mThread);
    mThread->start();
    QMetaObject::invokeMethod(mTiffSplit, "split", Qt::QueuedConnection);
}

void Importer::tiffSplitProgress(int progressValue)
{
    emit progress(progressValue);
}

void Importer::tiffSplitDone()
{
    emit done();
}
