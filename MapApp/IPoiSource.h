#ifndef IPOISOURCE_H
#define IPOISOURCE_H

#include <QList>
#include "Poi.h"

class IPoiSource
{
public:
    virtual const QList<Poi>& poiList() const = 0;
};

#endif // IPOISOURCE_H
