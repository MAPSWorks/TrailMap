/*
 * PoiListWidget.cpp
 *
 *  Created on: 14.4.2011
 *      Author: 
 */

#include "PoiListWidget.h"

#include <QVBoxLayout>

PoiListWidget::PoiListWidget(QWidget* parent) : 
    QWidget(parent)
{
    mListWidget = new QListWidget;
    mListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QVBoxLayout* l = new QVBoxLayout;
    l->setAlignment(Qt::AlignTop);
    l->addWidget(mListWidget);
    
    setLayout(l);
}

void PoiListWidget::addPoi(const Poi& poi)
{
    mListWidget->addItem(poi.title());
}

void PoiListWidget::clearPois()
{
    mListWidget->clear();
}
