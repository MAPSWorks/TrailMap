#ifndef MAPAPPLICATION_H
#define MAPAPPLICATION_H

#include <QApplication>

class MapApplication : public QApplication
{
    Q_OBJECT
public:
    explicit MapApplication(int & argc, char ** argv);
    
signals:
    void foregroundEvent(bool foreground);
    
public slots:
    
#ifdef Q_OS_SYMBIAN
    /*
protected:
    bool symbianEventFilter(const QSymbianEvent* symbianEvent);
    */
#endif
};

#endif // MAPAPPLICATION_H
