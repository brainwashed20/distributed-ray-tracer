#include "ThinRing.h"

namespace RayTracer
{
	void ThinRing::ObjectSpace_AppendAllIntersections(
		const Vector& vantage,
		const Vector& direction,
		std::vector<Intersection>& intersectionList) const
	{
		if(fabs(direction.z) > EPSILON)
		{
			const double u = -vantage.z / direction.z;
			if(u > EPSILON)
			{
				const double x = u*direction.x + vantage.x;
				const double y = u*direction.y + vantage.y;
				const double m = x*x + y*y;
				if((m <= r2*r2 + EPSILON) && (r1*r1 <= m + EPSILON))
				{
					Intersection intersection;
					intersection.point = Vector(x, y, 0.0);
					intersection.distanceSquared = (u * direction).MagnitudeSquared();

                    /**
					 * We "cheat" a little bit in calculating the normal vector by knowing too much about the caller.
					 * This is necessary because this is not really a normal solid object, but an infinitesimally thin surface.
					 * Therefore, we provide a different normal vector depending on the supplied vantage, 
					 * such that the point of view of the observer determines which side of the surface is seen.
					 * (Doing otherwise would cause the surface to appear completely black in some cases.)
					 */
                    intersection.surfaceNormal = Vector(0.0, 0.0, (vantage.z >= 0.0) ? +1.0 : -1.0);

					intersection.solid = this;
					intersectionList.push_back(intersection);
				}
			}
		}
	}
}