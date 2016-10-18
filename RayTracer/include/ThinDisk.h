#ifndef _THIN_DISK_H_
#define _THIN_DISK_H_

#include "ThinRing.h"

namespace RayTracer
{
	// A thin disc is a zero-thickness disc. It is implemented as a thin ring with a zero-radius hole.
	class ThinDisc: public ThinRing
	{
	public:

		ThinDisc(double _radius = 0.0)
			: ThinRing(0.0, _radius)
		{
			SetTag("ThinDisc");
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<ThinRing>(*this);
        }
	};
}

#endif