#ifndef _SET_UNION_H_
#define _SET_UNION_H_

#include "SolidObject_BinaryOperator.h"

namespace RayTracer
{
	class SetUnion: public SolidObject_BinaryOperator
	{
	public:
        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject_BinaryOperator>(*this);
        }

		SetUnion(const Vector& _center = Vector(), SolidObject* _left = nullptr, SolidObject* _right = nullptr)
			: SolidObject_BinaryOperator(_center, _left, _right)
		{
			SetTag("SetUnion");
		}

		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const;

		virtual bool Contains(const Vector& point) const
		{
			// A point is inside the set union if it is in either of the nested solids.
			return Left().Contains(point) || Right().Contains(point);
		}
	};
}

#endif