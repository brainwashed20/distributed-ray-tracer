

#include "Cylinder.h"

namespace RayTracer
{
	void Cylinder::ObjectSpace_AppendAllIntersections(
		const Vector& vantage,
		const Vector& direction,
		std::vector<Intersection>& intersectionList) const
	{
		// Look for intersections with the disks on the top and/or bottom of the cylinder.
		if(fabs(direction.z) > EPSILON)
		{
			AppendDiskIntersection(vantage, direction, +b, intersectionList);
			AppendDiskIntersection(vantage, direction, -b, intersectionList);
		}

		// Look for intersections with the curved lateral surface of the cylinder.
		double u[2];
		const int numRoots = 
            Algebra::SolveQuadraticEquation(
			    direction.x*direction.x + direction.y*direction.y,
			    2.0*(vantage.x*direction.x + vantage.y*direction.y),
			    vantage.x*vantage.x + vantage.y*vantage.y - a*a,
			    u);

		for(int i = 0; i < numRoots; ++i)
		{
			if(u[i] > EPSILON)
			{
				Intersection intersection;
				Vector displacement = u[i] * direction;
				intersection.point = vantage + displacement;

				/**
                 * We found an intersection with the infinitely-extended lateral surface,
				 * but the z-component must be within + or - b.
                 */
				if(fabs(intersection.point.z) <= b)
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(intersection.point.x, intersection.point.y, 0.0).UnitVector();
					intersection.solid = this;

					intersectionList.push_back(intersection);
				}
			}
		}
	}

	void Cylinder::AppendDiskIntersection(const Vector& vantage, const Vector& direction, double zDisk, std::vector<Intersection>& intersectionList) const
	{
		const double u = (zDisk - vantage.z) / direction.z;
		if(u > EPSILON)
		{
			Intersection intersection;
			Vector displacement = u * direction;
			intersection.point = vantage + displacement;
			
            const double x = intersection.point.x;
			const double y = intersection.point.y;
			
            if(x*x + y*y <= a*a)
			{
				intersection.distanceSquared = displacement.MagnitudeSquared();
				intersection.surfaceNormal = Vector(0.0, 0.0, (zDisk > 0.0) ? +1.0 : -1.0);
				intersection.solid = this;
				intersectionList.push_back(intersection);
			}
		}
	}
}