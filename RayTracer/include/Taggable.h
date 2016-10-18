#ifndef _TAGGABLE_H
#define _TAGGABLE_H

#include "Serialization.h"

namespace RayTracer
{
    // helps debugging; allows caller to assign names to things
	class Taggable      
	{
	public:
		Taggable(std::string _tag = std::string())
			: tag(_tag)
		{
		}

		void SetTag(std::string _tag)
		{
			tag = _tag;
		}

		std::string GetTag() const
		{
			return tag;
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & tag;
        }

		std::string tag;
	};
}

#endif