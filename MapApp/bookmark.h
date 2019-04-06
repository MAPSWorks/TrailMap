#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>
#include <QPoint>

class Bookmark
{
public:
    Bookmark(const QString& name, const QPoint& location);

public:
    void setName(const QString& name);
    void setLocation(const QPoint& location);

public:
    const QString& name() const;
    const QPoint& location() const;

private:
    QString mName;
    QPoint mLocation;
};

#endif // BOOKMARK_H
