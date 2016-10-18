#ifndef _THINRING_H_
#define _THINRING_H_

#include "SolidObject_Reorientable.h"

namespace RayTracer
{
	// A thin ring is a zero-thickness circular disc with an optional disc-shaped hole in the center.
	class ThinRing: public SolidObject_Reorientable
	{
	public:
		ThinRing(double _innerRadius = 0.0, double _outerRadius = 0.0)
			: SolidObject_Reorientable()
			, r1(_innerRadius)
			, r2(_outerRadius)
		{
			SetTag("ThinRing");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_Reorientable>(*this);
            ar & r1;
            ar & r1;
            ar & tempIntersectionList;
        }

	protected:

		virtual void ObjectSpace_AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool ObjectSpace_Contains(const Vector& point) const
		{
			if(fabs(point.z) <= EPSILON)
			{
				const double magSquared = point.x*point.x + point.y*point.y;
				return
					(r1*r1 <= EPSILON + magSquared) &&
					(magSquared <= EPSILON + r2*r2);
			}

			return false;
		}

	private:

        // The radius of the hole at the center of the ring.
		double  r1;     

        // The outer radius of the ring.
		double  r2;     
        
        /**
		 * A temporary intersection list, cached inside this object to avoid repeated memory allocations.
		 * Marked mutable to allow const functions to cache lists whose memory may be reused as needed.
		 */
        mutable std::vector<Intersection> tempIntersectionList;
	};
}

#endif