#include "settings.h"

#include <QSize>
#include <QDebug>

// Settings
const QString KSettingAdapter("adapter");
const QString KSettingLocation("location");
const QString KSettingZoom("zoom");
const QString KSettingHome("home");
const QString KSettingGPX("gpx");
const QString KSettingWindowState("windowState");
const QString KSettingWindowSize("windowSize");
const QString KSettingDataFolder("dataFolder");

Settings::Settings(QObject *parent) :
    QObject(parent)
{
}

void Settings::useAdapter(const QString& adapterName)
{
    Q_ASSERT(adapterName.indexOf("/") == -1);
    mAdapterName = adapterName + "/";
}

/*
 * GLOBAL SETTERS
 */
void Settings::saveAdapter(const QString& adapterName)
{
    mSettings.setValue(KSettingAdapter, QVariant(adapterName));
}

void Settings::saveWindowState(Qt::WindowStates windowState)
{
    mSettings.setValue(KSettingWindowState, QVariant(windowState));
}

void Settings::saveWindowSize(const QSize& windowSize)
{
    mSettings.setValue(KSettingWindowSize, windowSize);
}

/*
 * ADAPTER-SPECIFIC SETTERS
 */
void Settings::saveLocation(const QPoint& location)
{
    mSettings.setValue(mAdapterName + KSettingLocation, location);
}

void Settings::saveZoom(const int zoom)
{
    mSettings.setValue(mAdapterName + KSettingZoom, zoom);
}

void Settings::saveHomeLocation(const QPoint& location)
{
    mSettings.setValue(mAdapterName + KSettingHome, location);
}

void Settings::saveGPX(const QString& filePath)
{
    mSettings.setValue(mAdapterName + KSettingGPX, filePath);
}

void Settings::saveDataFolder(const QString& folderPath)
{
    mSettings.setValue(mAdapterName + KSettingDataFolder, folderPath);
}

/*
 * GETTERS
 */
const QString Settings::getAdapter() const
{
    return mSettings.value(KSettingAdapter, "").toString();
}

Qt::WindowStates Settings::getWindowState() const
{
    return (Qt::WindowStates) mSettings.value(KSettingWindowState, QVariant(Qt::WindowNoState)).toUInt();
}

const QSize Settings::getWindowSize() const
{
    return mSettings.value(KSettingWindowSize, QVariant(QSize(640, 400))).toSize();
}


/*
 * ADAPTER-SPECIFIC GETTERS
 */
const QPoint Settings::getLocation() const
{
    return mSettings.value(mAdapterName + KSettingLocation).toPoint();
}

const int Settings::getZoom() const
{
    return mSettings.value(mAdapterName + KSettingZoom).toInt();
}

const QPoint Settings::getHomeLocation() const
{
    return mSettings.value(mAdapterName + KSettingHome, QVariant(QPoint())).toPoint();
}

const QString Settings::getGPX() const
{
    return mSettings.value(mAdapterName + KSettingGPX).toString();
}

const QString Settings::getDataFolder() const
{
    return mSettings.value(mAdapterName + KSettingDataFolder).toString();
}
