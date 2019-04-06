/*
 * MapWidget.cpp
 *
 *  Created on: 11.4.2011
 *      Author: 
 */

#include "MapWidget.h"

#include <QPainter>
#include <QResizeEvent>
#include <QMath.h> 
#include <QStringList>
#include <QDir>
#include <QDebug>

const int KKeyMovementStep = 60*4;
const int KMaxCacheSize = 300;
const int KMaxZoom = 4;

MapWidget::MapWidget(QWidget* parent) : 
    QWidget(parent),
    mMapPixmap(NULL),
    mTileSize(0, 0),
    mAdapter(NULL),
    mDragging(false)
{
}

MapWidget::~MapWidget()
{
    delete mMapPixmap;
}

void MapWidget::setAdapter(IMapAdapter* adapter)
{
    if(mAdapter != NULL)
    {
        mAdapter->disconnect();
    }

    mAdapter = adapter;
    if(mAdapter)
    {
        mZoom = 1;
        mTileSize = mAdapter->tileSize();
        mMapPixmapLocation = mAdapter->defaultLocation();
        mLocation = QPoint(mMapPixmapLocation.x() * mTileSize.width(),
                           mMapPixmapLocation.y() * mTileSize.height());
        connect(mAdapter, SIGNAL(dataChanged()), SLOT(dataChanged()));
    }

    qDebug() << "setAdapter" << mMapPixmapLocation;

    if(mMapPixmap)
    {
        mMapPixmap->fill();
    }
}

IMapAdapter* MapWidget::adapter() const
{
    return mAdapter;
}

void MapWidget::setLocation(const QPoint& location)
{
    mLocation = QPoint(location.x() - mZoom * width() / 2, location.y() - mZoom * height() / 2);
    if(mMapPixmap)
    {
        adjustOffsetAndRender();
        update();
    }
}

void MapWidget::setLocation(const QPointF& latLon)
{
    const QPoint location = mAdapter->LatLonToDisplayCoordinate(latLon);
    setLocation(location);
}

QPoint MapWidget::location() const
{
    return mLocation;
}

QPoint MapWidget::mapCenter() const
{
    QPoint location(mLocation.x() + mZoom * width() / 2, mLocation.y() + mZoom * height() / 2);
    return location;
}

void MapWidget::keyPressEvent(QKeyEvent* event)
{
    const QPoint oldLocation = mLocation;
    
    switch(event->key())
    {
        case Qt::Key_Left:
            mLocation.rx() -= KKeyMovementStep;
            update();
            break;
        case Qt::Key_Right:
            mLocation.rx() += KKeyMovementStep;
            update();
            break;
        case Qt::Key_Up:
            mLocation.ry() -= KKeyMovementStep;
            update();
            break;
        case Qt::Key_Down:
            mLocation.ry() += KKeyMovementStep;
            update();
            break;
        default:
            event->ignore();
            break;
    }
    
    if(mLocation != oldLocation)
    {
        event->accept();
        adjustOffsetAndRender();
    }
}

void MapWidget::paintEvent(QPaintEvent* /*event*/)
{
    if(mMapPixmap)
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, *mMapPixmap, mOffset.x(), mOffset.y(), size().width(), size().height());
    }
}

void MapWidget::resizeEvent(QResizeEvent* event)
{
    delete mMapPixmap;
    
    if(mTileSize.isNull())
    {
        // Read tile size from file.
        const QPixmap pixmap = loadTile(QPoint(0, 0));
        mTileSize = pixmap.size();
    }
    if(mTileSize.isNull())
    {
        // Use default tile size if the file could not be found. 
        mTileSize = QSize(480, 480);
    }
    
    // Bitmap size is screen size + tile size rounded to higher multiple of tile size.
    QSize s(event->size().width() + mTileSize.width(),
            event->size().height() + mTileSize.height());
    s = QSize(qCeil((qreal) s.width() / mTileSize.width()),
              qCeil((qreal) s.height() / mTileSize.height()));
    s = QSize(s.width() * mTileSize.width(),
        s.height() * mTileSize.height());
    
    mMapPixmap = new QPixmap(s);
    
    render();
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(mDragging)
    {
        mLocation -= (event->pos() - mDragStart) * mZoom;
        mDragStart = event->pos();

        adjustOffsetAndRender();
        update();

        event->accept();
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        mDragging = true;
        mDragStart = event->pos();
        event->ignore();
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    mDragging = false;
    event->accept();
}

void MapWidget::wheelEvent(QWheelEvent* event)
{
    if(mAdapter->maxZoom() - mAdapter->minZoom() != 0 || !adapterZoom())
    {
        event->accept();


        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;

        if(!adapterZoom())
        {
            if(numSteps > 0)
            {
                zoomIn();
            }
            else if(numSteps < 0)
            {
                zoomOut();
            }
        }
        else
        {
            const int z = mAdapter->zoom() + numSteps;
            zoom(z);
        }
    }
}

void MapWidget::zoom(int zoom)
{
    if((zoom <= mAdapter->maxZoom() && zoom >= mAdapter->minZoom() && mAdapter->zoom() != zoom) || !adapterZoom())
    {
        if(adapterZoom())
        {
            QPoint displayCenter = QPoint(mLocation.x() + size().width() / 2,
                                          mLocation.y() + size().height() / 2);
            const QPointF latLon = mAdapter->DisplayCoordinateToLatLon(displayCenter);

            qDebug() << "Zoom:" << zoom;
            mAdapter->setZoom(zoom);

            // Re-position map
            displayCenter = mAdapter->LatLonToDisplayCoordinate(latLon);
            mLocation = QPoint(displayCenter.x() - size().width() / 2,
                               displayCenter.y() - size().height() / 2);
        }
        else
        {

        }

        render();
        update();

    }
}

bool MapWidget::adapterZoom()
{
   return mAdapter->minZoom() != mAdapter->maxZoom();
}

void MapWidget::render()
{
    QPoint tileLocation(mLocation.x() / mTileSize.width(),
                        mLocation.y() / mTileSize.height());
    
    // TODO: Hack-like fix.
    if(mLocation.x() < 0)
    {
        tileLocation.rx()--;
    }
    
    if(mLocation.y() < 0)
    {
        tileLocation.ry()--;
    }
    
    const QSize tiles(qCeil((qreal) mZoom * mMapPixmap->width() / mTileSize.width()),
                      qCeil((qreal) mZoom * mMapPixmap->height() / mTileSize.height()));
    
    QPainter painter(mMapPixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    
    for(int y = 0; y < tiles.height(); y++)
    {
        for(int x = 0; x < tiles.width(); x++)
        {
            const QPixmap pixmap = loadTile(QPoint(tileLocation.x() + x, tileLocation.y() + y));
            painter.drawPixmap(x * mTileSize.width() / mZoom, y * mTileSize.height() / mZoom, mTileSize.width() / mZoom, mTileSize.height() / mZoom, pixmap);
        }
    }
    
    mMapPixmapLocation = tileLocation;

    mOffset.setX((mLocation.x() - mMapPixmapLocation.x() * mTileSize.width()) / mZoom);
    mOffset.setY((mLocation.y() - mMapPixmapLocation.y() * mTileSize.height()) / mZoom);
}

void MapWidget::cycleZoom()
{
    int z = mAdapter->zoom();

    z++;

    if(z > mAdapter->maxZoom())
    {
        z = mAdapter->minZoom();
    }

    zoom(z);
}

void MapWidget::zoomIn()
{
    if(!adapterZoom())
    {
        // Save location.
        const QPoint loc = mapCenter();

        // Use realtime zoom.
        if(mZoom-- > 1)
        {
            zoom(mZoom);
        }

        mZoom = qMax(1, mZoom);

        setLocation(loc);
    }
    else
    {
        int z = mAdapter->zoom();

        if(++z <= mAdapter->maxZoom())
        {
            zoom(z);
        }

        mTileCache.clear();
    }
}

void MapWidget::zoomOut()
{
    if(!adapterZoom())
    {
        // Save location.
        const QPoint loc = mapCenter();

        // Use realtime zoom.
        if(mZoom++ < KMaxZoom)
        {
            zoom(mZoom);
        }

        mZoom = qMin(KMaxZoom, mZoom);

        setLocation(loc);
    }
    else
    {
        int z = mAdapter->zoom();

        if(--z >= mAdapter->minZoom())
        {
            zoom(z);
        }

        mTileCache.clear();
    }
}

inline uint qHash(const QPoint& p)
{
    return qHash(7 * p.x() + p.y());
}

QPixmap MapWidget::loadTile(const QPoint& tile) const
{
    QHash<QPoint, QPixmap>::const_iterator it = mTileCache.find(tile);
    if(it.key() == tile)
    {
        return it.value();
    }

    if(mTileCache.count() > KMaxCacheSize)
    {
        QHash<QPoint, QPixmap>* cache = const_cast<QHash<QPoint, QPixmap>*>(&mTileCache);
        cache->clear();         // TODO: remove the oldest.
    }

    QPixmap pixmap = mAdapter->loadTile(tile);

    if(pixmap.isNull())
    {
        QPixmap empty(mTileSize);
        empty.fill(Qt::darkGray);
        return empty;
    }
    else
    {
        QHash<QPoint, QPixmap>* cache = const_cast<QHash<QPoint, QPixmap>*>(&mTileCache);
        cache->insert(tile, pixmap);
    }

    return pixmap;
}

void MapWidget::adjustOffsetAndRender()
{
    mOffset.setX((mLocation.x() - mMapPixmapLocation.x() * mTileSize.width()) / mZoom);
    mOffset.setY((mLocation.y() - mMapPixmapLocation.y() * mTileSize.height()) / mZoom);

    if(mOffset.x() > mMapPixmap->size().width() - size().width() ||
       mOffset.y() > mMapPixmap->size().height() - size().height() ||
       mOffset.x() < 0 || mOffset.y() < 0)
    {
        // Render
        render();
    }
}

void MapWidget::dataChanged()
{
    // TODO: repaint only changed part of the bitmap.
    render();
    update();
}

void MapWidget::clearCache()
{
    mTileCache.clear();
}
