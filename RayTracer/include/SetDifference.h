#ifndef _SET_DIFFERENCE_H_
#define _SET_DIFFERENCE_H_

#include "SetIntersection.h"
#include "SetComplement.h"

namespace RayTracer
{
	// Set difference is a variation on set intersection: The difference A - B is identical to the intersection of A and not(B).
	class SetDifference: public SetIntersection
	{
	public:
		SetDifference(
			const Vector& _center = Vector(),
			SolidObject* _left = nullptr,
			SolidObject* _right = nullptr)
			: SetIntersection(_center, _left, new SetComplement(_right))
		{
			SetTag("SetDifference");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SetIntersection>(*this);
        }
	};
}

#endif