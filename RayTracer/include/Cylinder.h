#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "SolidObject_Reorientable.h"

namespace RayTracer
{
	/**
     * A circular, right cylinder (may be used to make "tin can" shapes, discs with thickness, etc.)
	 * Consists of a curved lateral surface and a top and bottom disc.
	 */
    class Cylinder: public SolidObject_Reorientable
	{
	public:
		Cylinder(double _radius = 0.0, double _height = 0.0)
			: SolidObject_Reorientable()
			, a(_radius)
			, b(_height / 2.0)
		{
			SetTag("Cylinder");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_Reorientable>(*this);
            ar & const_cast<double&> (a);
            ar & const_cast<double&> (b);
        }

	protected:
		virtual void ObjectSpace_AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool ObjectSpace_Contains(const Vector& point) const
		{
			return
				(fabs(point.z) <= b + EPSILON) &&
				(point.x*point.x + point.y*point.y <= a*a + EPSILON);
		}

	private:
		void AppendDiskIntersection(
			const Vector& vantage,
			const Vector& direction,
			double zDisk,
			std::vector<Intersection>& intersectionList) const;

        // the radius of the cylinder
		const double  a;  

        // half height of the cylinder
		const double  b; 
	};
}

#endif