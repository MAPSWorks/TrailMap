#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>

#include "TiffSplit.h"

class Importer : public QObject
{
    Q_OBJECT

public:
    Importer(QString&, QSize&, QObject*);

public slots:
    void import();
    void import(QString fileName);

    void tiffSplitProgress(int);
    void tiffSplitDone();
    //void finished();

signals:
    void progress(int);
    void done();

private:
    bool rmDir(const QString& dirName) const;

private:
    QThread* mThread;           // TODO: add cancel
    TiffSplit* mTiffSplit;      // TODO: add cancel

    QString mDestinationFolder;
    QSize mTileSize;
};

#endif // IMPORTER_H
