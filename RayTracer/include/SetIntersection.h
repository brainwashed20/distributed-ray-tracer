#ifndef _SETINTERSECTION_H_
#define _SETINTERSECTION_H_

#include "SolidObject_BinaryOperator.h"

namespace RayTracer
{
	class SetIntersection: public SolidObject_BinaryOperator
	{
	public:
        SetIntersection(
            const Vector& _center = Vector(),
            SolidObject* _left = nullptr,
            SolidObject* _right = nullptr)
            : SolidObject_BinaryOperator(_center, _left, _right)
        {
            SetTag("SetIntersection");
        }

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_BinaryOperator>(*this);
        }

		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool Contains(const Vector& point) const
		{
			// A point is inside the set intersection if it is inside both of the nested solids.
			return Left().Contains(point) && Right().Contains(point);
		}

	private:
     
		void AppendOverlappingIntersections(
			const Vector& vantage,
			const Vector& direction,
			const SolidObject& aSolid,
			const SolidObject& bSolid,
			std::vector<Intersection>& intersectionList) const;

		bool HasOverlappingIntersection(
			const Vector& vantage,
			const Vector& direction,
			const SolidObject& aSolid,
			const SolidObject& bSolid) const;
	};
}

#endif