#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QPoint>

class Settings : public QObject
{
public:
    explicit Settings(QObject *parent = 0);

public:
    void useAdapter(const QString& adapterName);

    void saveAdapter(const QString& adapterName);
    void saveWindowState(Qt::WindowStates windowState);
    void saveWindowSize(const QSize& windowSize);
    void saveLocation(const QPoint& location);
    void saveZoom(const int zoom);
    void saveHomeLocation(const QPoint& location);
    void saveGPX(const QString& filePath);
    void saveDataFolder(const QString& folderPath);

public:
    const QString getAdapter() const;
    Qt::WindowStates getWindowState() const;
    const QSize getWindowSize() const;
    const QPoint getLocation() const;
    const int getZoom() const;
    const QPoint getHomeLocation() const;
    const QString getGPX() const;
    const QString getDataFolder() const;


private:
    QSettings mSettings;
    QString mAdapterName;
};

#endif // SETTINGS_H
