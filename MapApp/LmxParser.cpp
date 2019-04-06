/*
 * GpxParser.cpp
 *
 *  Created on: 22.11.2012
 *      Author:
 */

#include "LmxParser.h"

LmxParser::LmxParser() :
    mInLandmark(false),
    mInName(false),
    mInCoordinates(false),
    mInLatitude(false),
    mInLongitude(false)
{
}

QList<Poi> LmxParser::Parse(const QString& fileName)
{
    QList<Poi> poiList;

    QFile file(fileName);
    QXmlInputSource source(&file);

    LmxParser handler;

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.parse(source);

    poiList = handler.poiList();

    return poiList;
}

Document* LmxParser::ParseDocument(const QString& fileName)
{
    QList<Route> routes;
    return new Document(Parse(fileName), routes, fileName);
}

const QList<Poi>& LmxParser::poiList() const
{
    return mPoiList;
}

bool LmxParser::startDocument()
{
    return true;
}

bool LmxParser::startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs)
{
    if(name == "lm:landmark")
    {
        mInLandmark = true;
    }
    else if(name == "lm:name")
    {
        mInName = true;
    }
    else if(name == "lm:coordinates")
    {
        mInCoordinates = true;
    }
    else if(name == "lm:latitude")
    {
        mInLatitude = true;
    }
    else if(name == "lm:longitude")
    {
        mInLongitude = true;
    }

    return true;
}

bool LmxParser::endElement(const QString&, const QString&, const QString &name)
{
    if(name == "lm:landmark")
    {
        mInLandmark = false;
        mPoiList.append(mPoi);
    }
    else if(name == "lm:name")
    {
        mInName = false;
    }
    else if(name == "lm:coordinates")
    {
        mInCoordinates = false;
    }
    else if(name == "lm:latitude")
    {
        mInLatitude = false;
    }
    else if(name == "lm:longitude")
    {
        mInLongitude = false;
    }

    return true;
}

bool LmxParser::characters(const QString & ch)
{
    if(mInLandmark)
    {
        if(mInName)
        {
            mPoi.setTitle(ch);
        }
        else if(mInCoordinates)
        {
            if(mInLatitude)
            {
                const qreal latitude = ch.toDouble();
                const QPointF location(mPoi.latLon().x(), latitude);
                mPoi.setLocation(location);
            }
            else if(mInLongitude)
            {
                const qreal longitude = ch.toDouble();
                const QPointF location(longitude, mPoi.latLon().y());
                mPoi.setLocation(location);
            }
        }
    }

    return true;
}
