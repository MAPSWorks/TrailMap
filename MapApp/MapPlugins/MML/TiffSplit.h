#ifndef TAB_TIFFSPLIT_H
#define TAB_TIFFSPLIT_H

#include <QObject>
#include <QString>

#include <QImageReader>

class TiffSplit : public QObject
{
    Q_OBJECT

public:
    TiffSplit(const QString& fileName, const QSize& tileSize, const QString& outputFolder, const int scale, const QRectF& geoRect, const qreal sourceScale);
    bool error() const;
    QString infoFile() const;

public slots:
    void split();

signals:
    void progress(int);
    void done();

private:
    QImage extractImage(const QRect& clipping);
    void saveTile(const QPoint& tileId, const QImage& image);
    void saveInfo(const QString& fileName, const QRect& tileRect);

private:
    QString mFileName;
    QString mOutputFolder;
    QSize mTileSize;
    QRectF mGeoRect;
    int mBitmapScale;
    int mSourceMapScale;
    bool mError;
};

#endif // TIFFSPLIT_H
