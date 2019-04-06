#ifndef IROUTESOURCE_H
#define IROUTESOURCE_H

#include <QList>
#include "route.h"

class IRouteSource
{
public:
    virtual const QList<Route>& routeList() const = 0;
};

#endif // IROUTESOURCE_H
