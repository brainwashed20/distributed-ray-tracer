#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "SolidObject.h"

namespace RayTracer
{
	class Sphere: public SolidObject
	{
	public:
		Sphere(const Vector& _center = Vector(), double _radius = 0.0)
			: SolidObject(_center)
			, radius(_radius)
		{
			SetTag("Sphere");   // tag for debugging
		}

		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool Contains(const Vector& point) const
		{
			/*
             * Add a little bit to the actual radius to be more tolerant
			 * of rounding errors that would incorrectly exclude a point that should be inside the sphere.
			 */
            const double r = radius + EPSILON;

			// A point is inside the sphere if the square of its distance from the center is within the square of the radius.
			return (point - Center()).MagnitudeSquared() <= (r * r);
		}

		// The nice thing about a sphere is that rotating it has no effect on its appearance!
		virtual SolidObject& RotateX(double angleInDegrees)
		{
			return *this;
		}

		virtual SolidObject& RotateY(double angleInDegrees)
		{
			return *this;
		}

		virtual SolidObject& RotateZ(double angleInDegrees)
		{
			return *this;
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject>(*this);
            ar & radius;
        }

	private:
      
		double  radius;
	};
}

#endif