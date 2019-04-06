/*
 * GpxWriter.h
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#ifndef GPXWRITER_H_
#define GPXWRITER_H_

#include <QFile>
#include <QPointF>
#include <QtXml/QDomDocument>

class GpxWriter
{
public:
    GpxWriter(const QString& fileName);
    virtual ~GpxWriter();
    
public:
    void addTrkpt(const QPointF& latlon);
    void addTrkpt(const QPointF& latlon, const qreal elevation);
    void addTrkpt(const QPointF& latlon, const qreal elevation, const qreal groundSpeed);

    // Don't mix addTrkpt() and addWpt() calls in the same file!
    void addWpt(const QPointF& latlon, const QString& name);

private:
    void initTrkpt();

private:
    QDomDocument mDocument;
    QDomElement mGpxElement; 
    QDomElement mTrkElement;
    QDomElement mTrksegElement;
    QDomElement mTrkptElement;
    QFile mFile;
    bool mTrkptDocument;
};

#endif /* GPXWRITER_H_ */
