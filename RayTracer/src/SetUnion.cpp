

#include "SetUnion.h"

namespace RayTracer
{
	void SetUnion::AppendAllIntersections(
		const Vector& vantage,
		const Vector& direction,
		std::vector<Intersection>& intersectionList) const
	{
		// Find all intersections with the left solid.
		Left().AppendAllIntersections(vantage, direction, intersectionList);

		// Append all intersections with the right solid.
		Right().AppendAllIntersections(vantage, direction, intersectionList);
	}
}