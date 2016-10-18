

#include "Cuboid.h"

namespace RayTracer
{
	void Cuboid::ObjectSpace_AppendAllIntersections(
		const Vector& vantage,
		const Vector& direction,
		std::vector<Intersection>& intersectionList) const
	{
		double u;
		Intersection intersection;
		Vector displacement;

		// Check for intersections with left/right faces: x = +a or x = -a.
		if(fabs(direction.x) > EPSILON)
		{
			// right face (x = +a)
			u = (a - vantage.x) / direction.x;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(+1.0, 0.0, 0.0);
					intersection.solid = this;
					intersection.tag = "right face";
					intersectionList.push_back(intersection);
				}
			}

			// left face (x = -a)
			u = (-a - vantage.x) / direction.x;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(-1.0, 0.0, 0.0);
					intersection.solid = this;
					intersection.tag = "left face";
					intersectionList.push_back(intersection);
				}
			}
		}

		// Check for intersections with front/back faces: y = -b or y = +b.
		if(fabs(direction.y) > EPSILON)
		{
			// front face (y = +b)
			u = (b - vantage.y) / direction.y;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(0.0, +1.0, 0.0);
					intersection.solid = this;
					intersection.tag = "front face";
					intersectionList.push_back(intersection);
				}
			}

			// back face (y = -b)
			u = (-b - vantage.y) / direction.y;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(0.0, -1.0, 0.0);
					intersection.solid = this;
					intersection.tag = "back face";
					intersectionList.push_back(intersection);
				}
			}
		}

		// Check for intersections with top/bottom faces: z = -c or z = +c.
		if(fabs(direction.z) > EPSILON)
		{
			// top face (z = +c)
			u = (c - vantage.z) / direction.z;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(0.0, 0.0, +1.0);
					intersection.solid = this;
					intersection.tag = "top face";
					intersectionList.push_back(intersection);
				}
			}

			// bottom face (z = -c)
			u = (-c - vantage.z) / direction.z;
			if(u > EPSILON)
			{
				displacement = u * direction;
				intersection.point = vantage + displacement;
				if(ObjectSpace_Contains(intersection.point))
				{
					intersection.distanceSquared = displacement.MagnitudeSquared();
					intersection.surfaceNormal = Vector(0.0, 0.0, -1.0);
					intersection.solid = this;
					intersection.tag = "bottom face";
					intersectionList.push_back(intersection);
				}
			}
		}
	}
}
