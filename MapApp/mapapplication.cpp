#include "mapapplication.h"

MapApplication::MapApplication(int& argc, char** argv) :
    QApplication(argc, argv)
{
}
#ifdef Q_OS_SYMBIAN
/*
// TODO: cannot build on plain QT.
bool MapApplication::symbianEventFilter(const QSymbianEvent* symbianEvent)
{
    const TWsEvent* event = symbianEvent->windowServerEvent();

    if(event)
    {
        switch(event->Type())
        {
            case EEventFocusGained:
            qDebug() << "Focus gained";
            iForeground = true;
            break;

            case EEventFocusLost:
            // Screensaver UID:
            qDebug() << "Focus lost ";
            iForeground = foregroundApp().iUid == KScreenSaverUid;
            break;

            default:
            break;
        }
    }

    return false;
}
*/
#endif
