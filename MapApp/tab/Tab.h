#ifndef TAB_H
#define TAB_H

/**
  *
  * Very simple MapInfo tab file parser.
  *
  */

#include <QObject>
#include <QString>
#include <QPointF>
#include <QList>
#include <QRectF>

class Tab
{
public:
    class CoordinatePair;

    Tab(const QString& imageFileName);
    bool isValid() const;

    QString imageFileName() const;
    //const QList<Tab::CoordinatePair>& coordinates() const;
    const QRectF& coordinateRect() const;

private:
    void parse(const QString& imageFileName);
    void parseLine(const QString& line);
    QString secondWord(const QString& line);
    void parseCoordinate(const QString& line);
    QString trimCoordinate(const QString& input) const;
    QPointF getPointF(const QString& input) const;

private:
    int mVersion;
    QString mFile;
    QString mType;
    bool mSignatureValid;
    QRectF mCoordinateRect;

public:
    class CoordinatePair
    {
    public:
        QPointF GeographicCoordinate, PixelCoordinate;

        CoordinatePair(const QPointF& geographicCoordinate, const QPointF& pixelCoordinate);
    };

private:
    QList<CoordinatePair> mCoordinateList;
};

#endif // TAB_H
