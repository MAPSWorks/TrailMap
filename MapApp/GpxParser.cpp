/*
 * GpxParser.cpp
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#include "GpxParser.h"

#include <QtXml/QXmlInputSource>

GpxParser::GpxParser() : 
    mInWpt(false),
    mInName(false)
{
    // TODO Auto-generated constructor stub

}

QList<Poi> GpxParser::Parse(const QString& fileName)
{
    QList<Poi> poiList;

    QFile file(fileName);
    QXmlInputSource source(&file);

    GpxParser handler;

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.parse(source);

    poiList = handler.poiList();

    return poiList;
}

Document* GpxParser::ParseDocument(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource source(&file);

    GpxParser handler;

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.parse(source);

    const QList<Poi> placemarks = handler.poiList();
    const QList<Route> routes = handler.routeList();

    Document* document = new Document(placemarks, routes, fileName);

    return document;
}

const QList<Poi>& GpxParser::poiList() const
{
    return mPoiList;
}

const QList<Route>& GpxParser::routeList() const
{
    return mRouteList;
}

bool GpxParser::startDocument()
{
    return true;
}

bool GpxParser::endElement(const QString&, const QString&, const QString &name)
{
    if(name == "wpt")
    {
        mInWpt = false;
        
        mPoiList.append(mPoi);
    }
    if(name == "name")
    {
        mInName = false;
    }
    if(name == "trk")
    {
        mInTrk = false;
    }
    if(name == "trkseg")
    {
        mInTrkSeg = false;
    }
    if(name == "rte")
    {
        mInRte = false;
    }
    if(name == "trkpt")
    {
        mInTrkPt = false;
    }

    return true;
}

bool GpxParser::startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs)
{
    if(name == "wpt")
    {
        mInWpt = true;
        
        const qreal lat = attrs.value("lat").toDouble();
        const qreal lon = attrs.value("lon").toDouble();
        
        mPoi = Poi(QPointF(lon, lat));
    }
    if(name == "name")
    {
        mInName = true;
    }
    if(name == "trk")
    {
        mInTrk = true;
    }
    if(name == "trkseg")
    {
        mInTrkSeg = true;

        Route* route = new Route();
        mRouteList.append(*route);
    }
    if(name == "rte")
    {
        mInRte = true;

        Route* route = new Route();
        mRouteList.append(*route);
    }
    if(name == "trkpt" || name == "rtept")
    {
        mInTrkPt = true;

        const qreal lat = attrs.value("lat").toDouble();
        const qreal lon = attrs.value("lon").toDouble();

        mRouteList.last().addPoint(QPointF(lon, lat));
    }

    return true;
}

bool GpxParser::characters(const QString & ch)
{
    if(mInName)
    {
        mPoi.setTitle(ch);
    }
    
    return true;
}
