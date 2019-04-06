/*
 * MapWidget.h
 *
 *  Created on: 11.4.2011
 *      Author: 
 */

#ifndef MAPWIDGET_H_
#define MAPWIDGET_H_

#include <QWidget>
#include <QPixmap>
#include <QSize>
#include <QPoint>
#include <QHash>

#include "imapadapter.h"

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    MapWidget(QWidget* parent = NULL);
    virtual ~MapWidget();
    
public:
    void setAdapter(IMapAdapter* adapter);
    IMapAdapter* adapter() const;
    void setLocation(const QPoint& location);
    void setLocation(const QPointF& latLon);  // Lat/lon
    QPoint location() const;
    QPoint mapCenter() const;
    void render();
    void cycleZoom();
    void zoomIn();
    void zoomOut();

public slots:
    void dataChanged();
    void clearCache();
    
protected:
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    
private:
    QPixmap loadTile(const QPoint& tile) const;
    void adjustOffsetAndRender();
    void zoom(int zoom);
    bool adapterZoom();
    
protected:
    QPoint mOffset;
    int mZoom;

private:
    IMapAdapter* mAdapter;      // REF

    //QString mDataFolder;
    QPixmap* mMapPixmap;
    QPoint mMapPixmapLocation;  // In tiles
    QPoint mLocation;           // Location of the viewport on the data in pixels.
    QSize mTileSize;            // TODO: move to adapter
    // For touchscreen
    QPoint mDragStart;
    //QPoint mDragOffset;
    bool mDragging;

    QHash<QPoint, QPixmap> mTileCache;
};

#endif /* MAPWIDGET_H_ */
