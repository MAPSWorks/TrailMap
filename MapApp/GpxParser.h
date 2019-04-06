/*
 * GpxParser.h
 *
 *  Created on: 12.4.2011
 *      Author: 
 */

#ifndef GPXPARSER_H_
#define GPXPARSER_H_

#include <QtXml/QXmlDefaultHandler>
#include <QList>

#include "Poi.h"
#include "document.h"

class GpxParser : public QXmlDefaultHandler
{
public:
    GpxParser();
    
public:
    static QList<Poi> Parse(const QString& fileName);
    static Document* ParseDocument(const QString& fileName);
    const QList<Poi>& poiList() const;
    const QList<Route>& routeList() const;
    
public: // From QXmlDefaultHandler
    bool startDocument();
    bool endElement(const QString&, const QString&, const QString &name);
    bool startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs);
    bool characters(const QString & ch);
    
private:
    bool mInWpt;
    bool mInName;
    bool mInTrk;
    bool mInTrkSeg;
    bool mInTrkPt;
    bool mInRte;
    Poi mPoi;
    QList<Poi> mPoiList;
    QList<Route> mRouteList;
};

#endif /* GPXPARSER_H_ */
