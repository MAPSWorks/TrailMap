#include "utility.h"

#include <QDir>
#include <QDebug>

#include "config.h"

/*
 * Look for tiledata in some known folders.
 **/
QString Utility::findDataFolder(const QString& provider)
{
    const QString KDataFolder(APP_NAME);

    QStringList paths;
    QStringList drives;

    paths.append(":/tiledata/");                        // For testing
    paths.append(":/" + KDataFolder + "/");             // Traditional location
    paths.append("/mnt/sdcard/" + KDataFolder + "/");   // For Android
    paths.append("/mnt/extSdCard/" + KDataFolder + "/");   // For Samsung Android

    drives.append("C");
    drives.append("D");
    drives.append("E");
    drives.append("F");

    for(int drive = 0; drive < drives.count(); drive++)
    {
        for(int path = 0; path < paths.count(); path++)
        {
            QString folder = paths.at(path);
            if(folder.left(1) == ":")
            {
                folder = drives.at(drive) + folder;
            }

            if(QDir(folder + provider).exists())
            {
                return folder + provider + "/";
            }

            if(QDir(folder).exists())
            {
                return folder;
            }
        }
    }

    qDebug() << "Data folder not found!";
    return QString();
}

QString Utility::findOrCreateDataFolder(const QString& provider)
{
    const QString folder = findDataFolder(provider);

    if(folder.isEmpty())
    {
        // TODO:
    }

    return folder;
}

QString Utility::findWorkingFolder()
{
    const QString KDataFolder(APP_NAME);

    QStringList paths;
    QStringList drives;

    paths.append("tiledata/");                           // Production location
    paths.append(":/tiledata/");                         // For testing
    paths.append(":/" + KDataFolder + "/");              // Traditional location
    paths.append("/mnt/sdcard/" + KDataFolder + "/");    // For Android
    paths.append("/mnt/extSdCard/" + KDataFolder + "/"); // For Samsung Android

    drives.append("C");
    drives.append("D");
    drives.append("E");
    drives.append("F");

    for(int drive = 0; drive < drives.count(); drive++)
    {
        for(int path = 0; path < paths.count(); path++)
        {
            QString folder = paths.at(path);
            if(folder.left(1) == ":")
            {
                folder = drives.at(drive) + folder;
            }

            if(QDir(folder).exists())
            {
                return folder;
            }
        }
    }

    qDebug() << "Data folder not found!";
    return QString();
}
