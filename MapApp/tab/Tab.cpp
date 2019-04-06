#include "Tab.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>

Tab::Tab(const QString& fileName) :
    mSignatureValid(false),
    mVersion(0)
{
    parse(fileName);
}

bool Tab::isValid() const
{
    return mSignatureValid && mVersion == 300 && mType == "Raster";
}

QString Tab::imageFileName() const
{
    return mFile;
}
/*
const QList<Tab::CoordinatePair>& Tab::coordinates() const
{
    return mCoordinateList;
}
*/

const QRectF& Tab::coordinateRect() const
{
    return mCoordinateRect;
}

void Tab::parse(const QString& fileName)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while(!in.atEnd())
       {
          QString line = in.readLine();
          parseLine(line);
       }
    }

    // We need 4 coordinates to specify the geo rect.
    if(mCoordinateList.length() == 4)
    {
        mCoordinateRect.setLeft(mCoordinateList.at(0).GeographicCoordinate.x());
        mCoordinateRect.setRight(mCoordinateList.at(1).GeographicCoordinate.x());
        mCoordinateRect.setTop(mCoordinateList.at(0).GeographicCoordinate.y());
        mCoordinateRect.setBottom(mCoordinateList.at(2).GeographicCoordinate.y());
    }

    inputFile.close();
}

void Tab::parseLine(const QString& line)
{
    if(line == "!table")
    {
        mSignatureValid = true;
    }
    else if(line.startsWith("!version"))
    {
        mVersion = secondWord(line).toInt();
    }
    else if(line.startsWith("File"))
    {
        mFile = secondWord(line).remove('\"');
    }
    else if(line.startsWith("Type"))
    {
        mType = secondWord(line).remove('\"');
    }
    else if(isValid() && mCoordinateList.length() < 4)
    {
        parseCoordinate(line);
    }
}

QString Tab::secondWord(const QString& line)
{
    QStringList words = line.split(' ');
    if(words.length() >= 2)
    {
        return words.at(1);
    }

    return QString();
}

void Tab::parseCoordinate(const QString& line)
{
    QStringList words = line.split(' ');

    if(words.length() >= 2)
    {
        QPointF p1 = getPointF(words.at(0));
        QPointF p2 = getPointF(words.at(1));

        CoordinatePair cp(p1, p2);
        mCoordinateList.append(cp);
    }
}

QString Tab::trimCoordinate(const QString& input) const
{
    QString result(input);
    result = result.remove('(');
    result = result.remove(')');
    return result;
}

QPointF Tab::getPointF(const QString& coordinate) const
{
    QPointF result;

    QString coord = trimCoordinate(coordinate);
    QStringList elements = coord.split(',');
    if(elements.length() == 2)
    {
        qreal h = elements.first().toDouble();
        qreal v = elements.last().toDouble();
        result = QPointF(h, v);
    }

    return result;
}

Tab::CoordinatePair::CoordinatePair(const QPointF& geographicCoordinate, const QPointF& pixelCoordinate)
{
    GeographicCoordinate = geographicCoordinate;
    PixelCoordinate = pixelCoordinate;
}


