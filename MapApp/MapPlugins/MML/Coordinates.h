//#include <e32math.h>

#include <QMath.h>
#include <QPoint>

/**
 *
 * Coordinate conversion utility class.
 *
 */
class KKJCoordinates
{
public:
    static QPoint LatLonToPixelL(const qreal aLat, const qreal aLon);
    static QPoint LatLonToKKJ(const QPointF& location);

    static qreal DegreeToRadian(const qreal aDegrees)
    {
        return aDegrees * M_PI / 180.0;
    }

    static qreal RadianToDegree(const qreal aRadians)
    {
        return aRadians * 180.0 / M_PI;
    }

private:
    static QPoint KKJLatLonToKKJxy(const qreal aLat, const qreal aLon, const int aZone, const qreal aLong0);
    static void WGSLatLonToKKJLatLon(const qreal aRadLat, const qreal aRadLon, qreal& aRadKKJLat, qreal& aRadKKJLon);

    static qreal cos(const qreal aParam);
    static qreal tan(const qreal aParam);
    static qreal sqrt(const qreal aParam);
    static qreal atan(const qreal aParam);
    static qreal log(const qreal aParam);
    static qreal sin(const qreal aParam);
};
