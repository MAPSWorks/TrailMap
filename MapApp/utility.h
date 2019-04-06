#ifndef UTILITY_H
#define UTILITY_H

#ifndef QTM_USE_NAMESPACE
#define QTM_USE_NAMESPACE
#endif

#include <QString>

class Utility
{
public:
    static QString findDataFolder(const QString& provider);
    static QString findOrCreateDataFolder(const QString& provider);
    static QString findWorkingFolder();
};

#endif // UTILITY_H
