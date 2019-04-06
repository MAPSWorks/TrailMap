#ifndef TFW_H
#define TFW_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QList>
#include <QRectF>

class Tfw
{
public:
    Tfw(const QString& fileName);
    QString imageFileName() const;
    QPointF baseCoordinate() const;
    QSizeF pixelSize() const;

private:

private:
    const QString mFileName;

    QSizeF mPixelSize;
    qreal mYRotation;
    qreal mXRotation;
    QPointF mTopLeftCoordinate;
};

#endif // TFW_H
