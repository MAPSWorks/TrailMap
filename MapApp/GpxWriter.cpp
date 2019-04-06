/*
 * GpxWriter.cpp
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#include "GpxWriter.h"

#include <QTextStream>

GpxWriter::GpxWriter(const QString& fileName) :
    mDocument("GPX"),
    mTrkptDocument(false)
{
    mGpxElement = mDocument.createElement("gpx");
    mDocument.appendChild(mGpxElement);
    
    mFile.setFileName(fileName);
    mFile.open(QIODevice::WriteOnly);           // TODO: Add error handling here.
}

GpxWriter::~GpxWriter()
{
    QTextStream stream(&mFile);
    stream << mDocument.toString(4);
    mFile.close();
}

void GpxWriter::addTrkpt(const QPointF& latlon)
{
    if(!mTrkptDocument)
        initTrkpt();

    mTrkptElement = mDocument.createElement("trkpt");
    mTrkptElement.setAttribute("lat", QString::number(latlon.y(), 'F', 6));
    mTrkptElement.setAttribute("lon", QString::number(latlon.x(), 'F', 6));
    mTrksegElement.appendChild(mTrkptElement);
}

void GpxWriter::addTrkpt(const QPointF& latlon, const qreal elevation)
{
    addTrkpt(latlon);
    
    QDomElement eleElement = mDocument.createElement("ele");
    QDomText text = mDocument.createTextNode(QString::number(elevation, 'F', 1));
    eleElement.appendChild(text);
    mTrkptElement.appendChild(eleElement);
}

void GpxWriter::addTrkpt(const QPointF& latlon, const qreal elevation, const qreal groundSpeed)
{
    addTrkpt(latlon, elevation);

    QDomElement speedElement = mDocument.createElement("speed");
    QDomText text = mDocument.createTextNode(QString::number(groundSpeed, 'F', 1));
    speedElement.appendChild(text);
    mTrkptElement.appendChild(speedElement);
}

void GpxWriter::addWpt(const QPointF& latlon, const QString& name)
{
    QDomElement element = mDocument.createElement("wpt");
    element.setAttribute("lat", QString::number(latlon.y(), 'F', 6));
    element.setAttribute("lon", QString::number(latlon.x(), 'F', 6));
    mGpxElement.appendChild(element);

    QDomElement nameElement = mDocument.createElement("name");
    QDomText text = mDocument.createTextNode(name);
    nameElement.appendChild(text);
    element.appendChild(nameElement);
}

void GpxWriter::initTrkpt()
{
    mTrkElement = mDocument.createElement("trk");
    mGpxElement.appendChild(mTrkElement);

    mTrksegElement = mDocument.createElement("trkseg");
    mTrkElement.appendChild(mTrksegElement);

    mTrkptDocument = true;
}
