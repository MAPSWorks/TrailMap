/*
 * LoggerWidget.h
 *
 *  Created on: 13.4.2011
 *      Author: 
 */

#ifndef LOGGERWIDGET_H_
#define LOGGERWIDGET_H_

#include <QWidget>
#include <QLabel>

class LoggerWidget : public QWidget
{
public:
    LoggerWidget(QWidget* parent = NULL);
    
public:
    void setDuration(const int duration);
    void setDistance(const qreal distance);
    void setAvgSpeed(const qreal speed);
    void setVelocity(const qreal velocity);
    
private:
    QLabel* mTimeLabel;
    QLabel* mDistanceLabel;
    QLabel* mAvgSpeedLabel;
    QLabel* mVelocityLabel;
};

#endif /* LOGGERWIDGET_H_ */
