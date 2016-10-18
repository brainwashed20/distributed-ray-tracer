#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "SolidObject.h"

#include <string>

namespace RayTracer
{
	class SolidObject;

	/** 
	 * struct Intersection provides information about a ray intersecting
	 * with a point on the surface of a SolidObject.
     */
	struct Intersection
	{
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & distanceSquared;
            ar & point;
            ar & surfaceNormal;
            ar & reinterpret_cast<SolidObject&> (solid);
            ar & reinterpret_cast<SolidObject&> (context);
            ar & tag;
        }

		// The square of the distance from the vantage point to the intersection point.
		double distanceSquared;

		// The location of the intersection point.
		Vector point;

		// The unit vector perpendicular to the surface at the intersection point.
		Vector surfaceNormal;

		// A pointer to the solid object that the ray intersected with.
		const SolidObject* solid;

        /**
		 * An optional tag for classes derived from SolidObject to cache  arbitrary information about surface optics.  
         * Most classes can safely leave this pointer as NULL, its default value.
		 */
        const void* context;

        /**
         * An optional tag used for debugging.
		 * Anything that finds an intersection may elect to make tag point at a static string to help the programmer figure out. 
         * For example, which of multiple surfaces was involved.  
         */
		std::string tag;

		// This constructor initializes to deterministic values in case some code forgets to initialize something.
		Intersection()
			: distanceSquared(1.0e+20)  // larger than any reasonable value
			, point()
			, surfaceNormal()
			, solid(nullptr)
			, context(nullptr)
			, tag(std::string())
		{
		}
	};

	int PickClosestIntersection(const std::vector<Intersection>& list, Intersection& intersection);
}

#endif