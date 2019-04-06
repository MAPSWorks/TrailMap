/*
 * LoggerWidget.cpp
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#include "LoggerWidget.h"

#include <QVBoxLayout>
#include <QTime>

const QString KDistance("Distance: %1 km");
const QString KDuration("Duration: %1");
const QString KAvgSpeed("Avg.speed: %1 km/h");
const QString KVelocity("Velocity: %1 km/h");

LoggerWidget::LoggerWidget(QWidget* parent) :
    QWidget(parent)
{
    // TODO Auto-generated constructor stub
    
    mTimeLabel = new QLabel("duration");
    mDistanceLabel = new QLabel("distance");
    mAvgSpeedLabel = new QLabel("avg.speed");
    mVelocityLabel = new QLabel("velocity");
    
    QVBoxLayout* l = new QVBoxLayout;
    l->setAlignment(Qt::AlignTop);
    l->addWidget(mTimeLabel);
    l->addWidget(mDistanceLabel);
    l->addWidget(mAvgSpeedLabel);
    l->addWidget(mVelocityLabel);
    setLayout(l);
}

void LoggerWidget::setDuration(const int duration)
{
    QTime time;
    time = time.addSecs(duration);
    mTimeLabel->setText(KDuration.arg(time.toString("h:mm:ss")));
}

void LoggerWidget::setDistance(const qreal distance)
{
    mDistanceLabel->setText(KDistance.arg(QString::number(distance / 1000, 'F', 1)));
}

void LoggerWidget::setAvgSpeed(const qreal speed)
{
    mAvgSpeedLabel->setText(KAvgSpeed.arg(QString::number(speed * 3.6, 'F', 1)));
}

void LoggerWidget::setVelocity(const qreal velocity)
{
    mVelocityLabel->setText(KVelocity.arg(QString::number(velocity * 3.6, 'F', 1)));
}
