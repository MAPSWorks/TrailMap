#ifndef ROUTE_H
#define ROUTE_H

#include <QList>
#include <QPointF>

class Route
{
public:
    Route();
    Route(const QList<QPointF>& points);
    void addPoint(const QPointF& point);
    const QList<QPointF>& points() const;

private:
    QList<QPointF> mPointList;
};

#endif // ROUTE_H
