
#include "TiffSplit.h"

#include <QDir>
#include <QXmlStreamWriter>
#include <QtCore/qmath.h>

TiffSplit::TiffSplit(const QString& fileName, const QSize& tileSize, const QString& outputFolder, const int scale, const QRectF& geoRect, const qreal sourceScale) :
    mFileName(fileName),
    mOutputFolder(outputFolder),
    mTileSize(tileSize),
    mBitmapScale(scale),
    mGeoRect(geoRect),
    mSourceMapScale(sourceScale),
    mError(false)
{
}

void TiffSplit::split()
{
    QImageReader reader(mFileName);
    const QSize sizeInTiles(reader.size().width() / (mTileSize.width() * mBitmapScale),
                            reader.size().height() / (mTileSize.height() * mBitmapScale));

    //const QPointF KOrigo(452001.25, 7241998.75);         // Top-left in file UR444_RVK_25.tfw is the origo.
    const QPointF KOrigo(20006.25, 7817993.75);            // New

    const QPointF p = QPointF(mGeoRect.left() - KOrigo.x(),
                              mGeoRect.top() - KOrigo.y());

    const QPointF baseTileF(p.x() / (mGeoRect.width() / (qreal)sizeInTiles.width()),
                            p.y() / (mGeoRect.height() / (qreal)sizeInTiles.height()));

    const QPoint baseTile(qRound(baseTileF.x()), qRound(baseTileF.y()));

    for(int y = 0; y < sizeInTiles.height(); y++)
    {
        for(int x = 0; x < sizeInTiles.width(); x++)
        {
            const QRect clip(x * mTileSize.width(), y * mTileSize.height(), mTileSize.width(), mTileSize.height());
            QImage image = extractImage(clip);
            if(image.isNull())
            {
                mError = true;
                break;
            }
            saveTile(QPoint(x + baseTile.x(), y + baseTile.y()), image);

            // Report progress
            emit progress(100 * (x + y * sizeInTiles.width()) / (sizeInTiles.width() * sizeInTiles.height()));
        }
    }

    saveInfo(infoFile(), QRect(baseTile, sizeInTiles));

    emit done();
}

bool TiffSplit::error() const
{
    return mError;
}

QString TiffSplit::infoFile() const
{
    QFileInfo fileInfo(mFileName);
    return mOutputFolder + fileInfo.baseName() + ".tiles";
}

QImage TiffSplit::extractImage(const QRect& clipping)
{
    QImageReader reader(mFileName);
    reader.setScaledSize(QSize(reader.size().width() / mBitmapScale, reader.size().height() / mBitmapScale));
    reader.setScaledClipRect(clipping);
    return reader.read();
}

void TiffSplit::saveTile(const QPoint& tileId, const QImage& image)
{
    QString tilePath(mOutputFolder + "%1/%2.png");
    tilePath = tilePath.arg(tileId.x()).arg(tileId.y());

    QFileInfo fileInfo(tilePath);
    QDir dir;
    dir.mkpath(fileInfo.absolutePath());
    image.save(tilePath);
}

void TiffSplit::saveInfo(const QString& fileName, const QRect& tileRect)
{
    // TODO: save xml

    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter writer;
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(-4);

        writer.setDevice(&file);
        writer.writeStartDocument();

        writer.writeComment("Tile folder info file");
        writer.writeStartElement("TileInfo");

        writer.writeStartElement("AbsolutePath");
        writer.writeCharacters("");
        writer.writeEndElement();

        writer.writeStartElement("Scale");
        writer.writeCharacters(QString::number(mBitmapScale));
        writer.writeEndElement();

        writer.writeStartElement("TileSize");
        writer.writeCharacters(QString::number(mTileSize.width()));
        writer.writeEndElement();

        writer.writeStartElement("Left");
        writer.writeCharacters(QString::number(tileRect.left()));
        writer.writeEndElement();

        writer.writeStartElement("Top");
        writer.writeCharacters(QString::number(tileRect.top()));
        writer.writeEndElement();

        writer.writeStartElement("Right");
        writer.writeCharacters(QString::number(tileRect.right()));
        writer.writeEndElement();

        writer.writeStartElement("Bottom");
        writer.writeCharacters(QString::number(tileRect.bottom()));
        writer.writeEndElement();

        writer.writeStartElement("SourceMapScale");
        writer.writeCharacters(QString::number(mSourceMapScale));
        writer.writeEndElement();

        writer.writeEndElement();   // TileInfo
        writer.writeEndDocument();

        file.close();
    }
}
