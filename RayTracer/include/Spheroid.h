#ifndef _SPHEROID_H_
#define _SPHEROID_H_

#include "SolidObject_Reorientable.h"

namespace RayTracer
{
	// A sphere-like object, only with different dimensions allowed in the x, y, and z directions.
	class Spheroid: public SolidObject_Reorientable
	{
	public:
		Spheroid(double _a = 0.0, double _b = 0.0, double _c = 0.0)
			: SolidObject_Reorientable()
			, a(_a)
			, b(_b)
			, c(_c)
			, a2(_a * _a)
			, b2(_b * _b)
			, c2(_c * _c)
		{
			SetTag("Spheroid");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_Reorientable>(*this);
            ar & const_cast<double&> (a);
            ar & const_cast<double&> (b);
            ar & const_cast<double&> (c);
            ar & const_cast<double&> (a2);
            ar & const_cast<double&> (b2);
            ar & const_cast<double&> (c2);
        }

	protected:
		virtual void ObjectSpace_AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool ObjectSpace_Contains(const Vector& point) const
		{
			const double xr = point.x / a;
			const double yr = point.y / b;
			const double zr = point.z / b;
			return xr*xr + yr*yr + zr*zr <= 1.0 + EPSILON;
		}

	private:
      
		const double  a;      // radius along the x-axis
		const double  b;      // radius along the y-axis
		const double  c;      // radius along the z-axis

		const double  a2;     // a*a, cached for efficiency
		const double  b2;     // b*b, cached for efficiency
		const double  c2;     // c*c, cached for efficiency
	};
}

#endif