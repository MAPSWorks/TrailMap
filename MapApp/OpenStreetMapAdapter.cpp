#include "OpenStreetMapAdapter.h"

const QString KAdapterName("OpenStreetMap");
const QString KHost("http://otile1.mqcdn.com/tiles/1.0.0/osm");
const QString KFileNameFormat("%1/%2/%3.jpg");
const QString KCopyrightNotice("Data, imagery and map information provided by MapQuest, Open Street Map <http://www.openstreetmap.org/> and contributors, CC-BY-SA <http://creativecommons.org/licenses/by-sa/2.0/> .");

OpenStreetMapAdapter::OpenStreetMapAdapter(QObject* parent) :
    OpenStreetMapCompatibleAdapter(parent, KAdapterName, 15, 6, IMapAdapter::None, KFileNameFormat)
{
    mHost = KHost;
    setCopyrightNotice(KCopyrightNotice);
}

bool OpenStreetMapAdapter::openDataSource(QString& /*url*/)
{
    QString host(KHost);
    return OpenStreetMapCompatibleAdapter::openDataSource(host);
}
