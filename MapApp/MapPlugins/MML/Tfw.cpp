#include "Tfw.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QFileInfo>

// SEE: http://en.wikipedia.org/wiki/World_file

Tfw::Tfw(const QString& fileName) :
    mFileName(fileName)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QStringList params;

        QTextStream in(&inputFile);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            params.append(line);
        }

        mPixelSize = QSizeF(params.at(0).toDouble(), params.at(3).toDouble());
        mYRotation = params.at(1).toDouble();
        mXRotation = params.at(2).toDouble();
        mTopLeftCoordinate = QPointF(params.at(4).toDouble(), params.at(5).toDouble());
    }
}

QString Tfw::imageFileName() const
{
    QFileInfo fileInfo(mFileName);
    return fileInfo.baseName() + ".tif";
}

QPointF Tfw::baseCoordinate() const
{
    return mTopLeftCoordinate;
}

QSizeF Tfw::pixelSize() const
{
    return mPixelSize;
}
