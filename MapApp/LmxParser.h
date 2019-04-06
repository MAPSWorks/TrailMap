#ifndef LMXPARSER_H
#define LMXPARSER_H

#include <QtXml/QXmlDefaultHandler>
#include <QList>

#include "Poi.h"
#include "document.h"

class LmxParser : public QXmlDefaultHandler
{
public:
    LmxParser();

public:
    static QList<Poi> Parse(const QString& fileName);
    static Document* ParseDocument(const QString& fileName);
    const QList<Poi>& poiList() const;

public: // From QXmlDefaultHandler
    bool startDocument();
    bool endElement(const QString&, const QString&, const QString &name);
    bool startElement(const QString&, const QString&, const QString &name, const QXmlAttributes &attrs);
    bool characters(const QString & ch);

private:
    bool mInLandmark;
    bool mInName;
    bool mInCoordinates;
    bool mInLatitude;
    bool mInLongitude;

    Poi mPoi;
    QList<Poi> mPoiList;
};

#endif // LMXPARSER_H
