#ifndef MAPINFOTABADAPTER_H
#define MAPINFOTABADAPTER_H

#include <QObject>
#include <QThread>

#include "IMapAdapter.h"
#include "maanmittauslaitosmapadapter.h"
#include "tab/TiffSplit.h"

class MapInfoTabAdapter : public MaanmittauslaitosMapAdapter
{
    Q_OBJECT

public:
    MapInfoTabAdapter(QObject *parent = 0);

public:
    QString name();
    bool openDataSource(QString& path);
    void unload();

public slots:
    void tiffSplitProgress(int);
    void tiffSplitDone();
    void finished();

private:
    bool rmDir(const QString& dirName) const;

private:
    QThread* mThread;           // TODO: add cancel
    TiffSplit* mTiffSplit;      // TODO: add cancel
};

#endif // MAPINFOTABADAPTER_H
