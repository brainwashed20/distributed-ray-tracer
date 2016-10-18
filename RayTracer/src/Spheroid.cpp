#include "Spheroid.h"

namespace RayTracer
{
	void Spheroid::ObjectSpace_AppendAllIntersections(
		const Vector& vantage,
		const Vector& direction,
		std::vector<Intersection>& intersectionList) const
	{
		double u[2];
		const int numSolutions = Algebra::SolveQuadraticEquation(
			b2*c2*direction.x*direction.x + a2*c2*direction.y*direction.y + a2*b2*direction.z*direction.z,
			2.0*(b2*c2*vantage.x*direction.x + a2*c2*vantage.y*direction.y + a2*b2*vantage.z*direction.z),
			b2*c2*vantage.x*vantage.x + a2*c2*vantage.y*vantage.y + a2*b2*vantage.z*vantage.z - a2*b2*c2,
			u
		);

		for(int i = 0; i < numSolutions; ++i)
		{
			if(u[i] > EPSILON)
			{
				Intersection intersection;
				Vector displacement = u[i] * direction;
				intersection.distanceSquared = displacement.MagnitudeSquared();
				intersection.point = vantage + displacement;

                /**
				 * The surface normal vector was calculated by expressing the spheroid as a 
				 * function z(x,y) = sqrt(1 - (x/a)^2 - (y/b)^2),
				 * taking partial derivatives dz/dx = (c*c*x)/(a*a*z), dz/dy = (c*c*y)/(b*b*z), 
				 * and using these to calculate the vectors <1, 0, dz/dx> and <0, 1, dy,dz>.
				 * The normalized cross product of these two vectors yields the surface normal vector.
				 */
                const double x = intersection.point.x;
				const double y = intersection.point.y;
				const double z = intersection.point.z;

				// But we need to handle special cases when z is very close to 0.
				if(fabs(z) <= EPSILON)
				{
					if(fabs(x) <= EPSILON)
					{
						// The equation devolves to (y^2)/(b^2) = 1, or y = +/- b.
						intersection.surfaceNormal = Vector(0.0, ((y > 0.0) ? 1.0 : -1.0), 0.0);
					}
					else
					{
						// The equation devolves to an ellipse on the xy plane : 
						// (x^2)/(a^2) + (y^2)/(b^2) = 1.
						intersection.surfaceNormal = Vector(-1.0, -(a2*y) / (b2*x), 0.0).UnitVector();
					}
				}
				else
				{
					intersection.surfaceNormal = Vector((c2*x) / (a2*z), (c2*y) / (b2*z), 1.0).UnitVector();
				}

                /**
				 * Handle special cases with polarity: the polarity of the components of
				 * the surface normal vector must match that of the <x,y,z> intersection point,
				 * because the surface normal vector always points (roughly) away from the vantage, just
				 * like any point on the surface of the spheroid does.
                 */
				if(x * intersection.surfaceNormal.x < 0.0)     // negative product means opposite polarities
				{
					intersection.surfaceNormal.x *= -1.0;
				}

				if(y * intersection.surfaceNormal.y < 0.0)     // negative product means opposite polarities
				{
					intersection.surfaceNormal.y *= -1.0;
				}

				if(z * intersection.surfaceNormal.z < 0.0)     // negative product means opposite polarities
				{
					intersection.surfaceNormal.z *= -1.0;
				}

				intersection.solid = this;

				intersectionList.push_back(intersection);
			}
		}
	}
}
