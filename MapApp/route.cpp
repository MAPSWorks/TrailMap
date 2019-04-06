#include "route.h"

Route::Route()
{
}

Route::Route(const QList<QPointF>& points) :
    mPointList(points)
{
}

void Route::addPoint(const QPointF& point)
{
    mPointList.append(point);
}

const QList<QPointF>& Route::points() const
{
    return mPointList;
}
