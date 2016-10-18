#ifndef _SETCOMPLEMENT_H_
#define _SETCOMPLEMENT_H_

#include "SolidObject.h"

namespace RayTracer
{
    /**
	 * Filters a SolidObject, except toggles the inside/outside property.
	 * For example, a sphere becomes the set of points outside the sphere.
	 */
    class SetComplement: public SolidObject
	{
	public:
		explicit SetComplement(SolidObject* _other = nullptr)
			: SolidObject((_other != nullptr) ? _other->Center() : Vector())
			, other(_other)
		{
			SetTag("SetComplement");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject>(*this);
            ar & other;
        }

		virtual ~SetComplement()
		{
			delete other;
			other = NULL;
		}

		virtual bool Contains(const Vector& point) const
		{
			// This is the core of the set complement: toggling the value of any point containment.
			return !other->Contains(point);
		}

		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual SolidObject& Translate(double dx, double dy, double dz)
		{
			SolidObject::Translate(dx, dy, dz);
			other->Translate(dx, dy, dz);
			return *this;
		}

		virtual SolidObject& RotateX(double angleInDegrees)
		{
			other->RotateX(angleInDegrees);
			return *this;
		}

		virtual SolidObject& RotateY(double angleInDegrees)
		{
			other->RotateY(angleInDegrees);
			return *this;
		}

		virtual SolidObject& RotateZ(double angleInDegrees)
		{
			other->RotateZ(angleInDegrees);
			return *this;
		}

	private:
       
		SolidObject* other;
	};
}

#endif