#include "bookmark.h"

Bookmark::Bookmark(const QString& name, const QPoint& location) :
    mName(name),
    mLocation(location)
{
}

void Bookmark::setName(const QString& name)
{
    mName = name;
}

void Bookmark::setLocation(const QPoint& location)
{
    mLocation = location;
}

const QString& Bookmark::name() const
{
    return mName;
}

const QPoint& Bookmark::location() const
{
    return mLocation;
}
