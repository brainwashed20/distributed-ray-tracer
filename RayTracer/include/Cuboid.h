#ifndef _CUBOID_H_
#define _CUBOID_H_

#include "SolidObject_Reorientable.h"

namespace RayTracer
{
	// A box with rectangular faces, all of which are mutually perpendicular.
	class Cuboid: public SolidObject_Reorientable
	{
	public:

		Cuboid(double _a = 0.0, double _b = 0.0, double _c = 0.0)
			: SolidObject_Reorientable()
			, a(_a)
			, b(_b)
			, c(_c)
		{
			SetTag("Cuboid");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_Reorientable>(*this);
            ar & const_cast<double&> (a);
            ar & const_cast<double&> (b);
            ar & const_cast<double&> (c);
        }

	protected:

		virtual void ObjectSpace_AppendAllIntersections(const Vector& vantage, const Vector& direction, std::vector<Intersection>& intersectionList) const;

		virtual bool ObjectSpace_Contains(const Vector& point) const
		{
			return
				(fabs(point.x) <= a + EPSILON) &&
				(fabs(point.y) <= b + EPSILON) &&
				(fabs(point.z) <= c + EPSILON);
		}

	private:
        
		const double  a;   // half of the width:  faces at r = -a and r = +a.
		const double  b;   // half of the length: faces at s = -b and s = +b.
		const double  c;   // half of the height: faces at t = -c and t = +c.
	};
}

#endif