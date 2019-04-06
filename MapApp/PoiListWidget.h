/*
 * PoiListWidget.h
 *
 *  Created on: 14.4.2011
 *      Author: 
 */

#ifndef POILISTWIDGET_H_
#define POILISTWIDGET_H_

#include <QWidget>
#include <QListWidget>

#include "Poi.h"

class PoiListWidget : public QWidget
{
public:
    PoiListWidget(QWidget* parent = NULL);
    
public:
    void addPoi(const Poi& poi);
    void clearPois();
    
private:
    QListWidget* mListWidget;
};

#endif /* POILISTWIDGET_H_ */
