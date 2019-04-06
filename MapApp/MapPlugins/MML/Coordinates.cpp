#include "Coordinates.h"
#include <QSize>

/*
 * Coordinate conversion functions based on:
 * http://www.viestikallio.fi/tools/kkj-wgs84.php
 */

QPoint KKJCoordinates::LatLonToPixelL(const qreal aLat, const qreal aLon)
{
        const QSize m_sizeBlock(480, 480);	// Tile size in pixels
	
        const qreal radLat = DegreeToRadian(aLat);
        const qreal radLon = DegreeToRadian(aLon);

	if((18.5 < aLon && aLon < 32.0) && (59.0 < aLat && aLat < 70.5))
	{
                qreal kkjLat;
                qreal kkjLon;
		WGSLatLonToKKJLatLon(radLat, radLon, kkjLat, kkjLon);
		
                int zone = 3;
                const qreal long0 = DegreeToRadian(zone * 3 + 18);

                const QPoint KKJ = KKJLatLonToKKJxy(kkjLat, kkjLon, zone, long0);

		// Convert KKJ to local pixel coordinate.
		// First bias to origo.
                const QPoint KWorldOrigo(3454345, 7216161);
                QPoint result(KKJ - KWorldOrigo);
                result.setY(result.y() * -1);
		
        // Convert to tile.
                const qreal fPixelX = (qreal) result.x() / 1920;
                const qreal fPixelY = (qreal) result.y() / 1920;

		// Convert to local pixel
                result = QPoint(fPixelX * m_sizeBlock.width() + 300, fPixelY * m_sizeBlock.height() + 280);
		
		return result;
	}

        //User::Leave(KErrArgument);
        return QPoint();
}

QPoint KKJCoordinates::LatLonToKKJ(const QPointF& location)
{
    const qreal aLat = location.y();
    const qreal aLon = location.x();

    const qreal radLat = DegreeToRadian(aLat);
    const qreal radLon = DegreeToRadian(aLon);

    if((18.5 < aLon && aLon < 32.0) && (59.0 < aLat && aLat < 70.5))
    {
        qreal kkjLat;
        qreal kkjLon;
        WGSLatLonToKKJLatLon(radLat, radLon, kkjLat, kkjLon);

        int zone = 3;
        const qreal long0 = DegreeToRadian(zone * 3 + 18);

        const QPoint KKJ = KKJLatLonToKKJxy(kkjLat, kkjLon, zone, long0);


        return KKJ;
    }

    return QPoint();
}

qreal KKJCoordinates::cos(const qreal aParam)
{
        return qCos(aParam);
}

qreal KKJCoordinates::tan(const qreal aParam)
{
    return qTan(aParam);
}

qreal KKJCoordinates::sqrt(const qreal aParam)
{
    return qSqrt(aParam);
}

qreal KKJCoordinates::atan(const qreal aParam)
{
    return qAtan(aParam);
}

qreal KKJCoordinates::log(const qreal aParam)
{
    return qLn(aParam);
}

qreal KKJCoordinates::sin(const qreal aParam)
{
    return qSin(aParam);
}

QPoint KKJCoordinates::KKJLatLonToKKJxy(const qreal aLat, const qreal aLon, const int aZone, const qreal aLong0)
{
/*
# Inputs:  $ED50[Lo] = Longitude (Radians)
#          $ED50[La] = Latitude  (Radians)
# Outputs: $ED50[X], $ED50[Y]  (meters)
*/
        const qreal lon = aLon - aLong0;

        const qreal a = 6378388.0;
        const qreal f = 1/297.0;

        const qreal b = (1.0 - f) * a;
        const qreal bb = b * b;
        const qreal c = (a / b) * a;
        const qreal ee = (a * a - bb) / bb;
        const qreal n = (a - b)/(a + b);

        const qreal nn = n * n;

        const qreal cosLat = cos(aLat);

        const qreal NN = ee * cosLat * cosLat;

        const qreal LaF = atan(tan(aLat) / cos(lon * sqrt(1 + NN)));

        const qreal cosLaF = cos(LaF);

        const qreal t  = (tan(lon) * cosLaF) / sqrt(1 + ee * cosLaF * cosLaF);

        const qreal A = a / ( 1 + n );

        const qreal A1 = A * (1 + nn / 4 + nn * nn / 64);

        const qreal A2 = A * 1.5 * n * (1 - nn / 8);

        const qreal A3 = A * 0.9375 * nn * (1 - nn / 4);

        const qreal A4 = A * 35/48.0 * nn * n;

        const QPoint result = QPoint(
				c * log(t + sqrt(1 + t * t)) +
				500000.0 + aZone * 1000000.0,
				A1 * LaF -
                A2 * sin(2 * LaF) +
                A3 * sin(4 * LaF) -
                A4 * sin(6 * LaF)
				);

	return result;
}

void KKJCoordinates::WGSLatLonToKKJLatLon(const qreal aRadLat, const qreal aRadLon, qreal& aRadKKJLat, qreal& aRadKKJLon)
{
        const qreal radLat = RadianToDegree(aRadLat);
        const qreal radLon = RadianToDegree(aRadLon);

        const qreal dRadLat = DegreeToRadian( -0.124766E01 +
                    0.269941E00 * radLat +
                    -0.191342E00 * radLon +
                    -0.356086E-02 * radLat * radLat +
                    0.122353E-02 * radLat * radLon +
                    0.335456E-03 * radLon * radLon) / 3600.0;

        const qreal dRadLon = DegreeToRadian(0.286008E02 +
                     -0.114139E01 * radLat +
                     0.581329E00 * radLon +
                     0.152376E-01 * radLat * radLat +
                     -0.118166E-01 * radLat * radLon +
                     -0.826201E-03 * radLon * radLon) / 3600.0;

	aRadKKJLat = aRadLat + dRadLat;
	aRadKKJLon = aRadLon + dRadLon;
}
