#ifndef KARTTAPAIKKAMAPADAPTER_H
#define KARTTAPAIKKAMAPADAPTER_H

#include <QObject>
#include <QSize>

#include "IMapAdapter.h"

class KarttapaikkaMapAdapter : public IMapAdapter
{
public:
    KarttapaikkaMapAdapter(QObject* parent = NULL);

public:
    QString name();
    TDataSourceType DataSourceType();
    bool openDataSource(QString& path);
    QPoint LatLonToDisplayCoordinate(const QPointF& coordinate);
    QPointF DisplayCoordinateToLatLon(const QPoint& coordinate);
    QPixmap loadTile(const QPoint& tile);
};

#endif // KARTTAPAIKKAMAPADAPTER_H
