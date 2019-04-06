#ifndef OPENSTREETMAPADAPTER_H
#define OPENSTREETMAPADAPTER_H

#include "OpenStreetMapCompatibleAdapter.h"

class OpenStreetMapAdapter : public OpenStreetMapCompatibleAdapter
{
public:
    OpenStreetMapAdapter(QObject* parent = NULL);

public:
    bool openDataSource(QString& url);
};

#endif // OPENSTREETMAPADAPTER_H
