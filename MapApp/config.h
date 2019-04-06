#ifndef CONFIG_H
#define CONFIG_H

#include <QDebug>

#define APP_NAME "TrailMap"
#define USER_AGENT "TrailMap/0.0-beta"

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_ANDROID) || defined(Q_WS_SIMULATOR)
# define MAP_ON_MOBILE
#endif

#endif // CONFIG_H
