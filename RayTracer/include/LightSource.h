#ifndef _LIGHT_SOURCE_H_
#define _LIGHT_SOURCE_H_

#include "Taggable.h"
#include "Vector.h"
#include "Color.h"

namespace RayTracer
{
	/**
     * For now, all light sources are single points with an inherent color.
	 * Luminosity of the light source can be changed by multiplying
	 * color.red, color.green, color.blue all by a constant value.
	 */
    class LightSource: public Taggable
	{
    public:
		LightSource(const Vector& _location = Vector(), const Color& _color = Color(0.0, 0.0, 0.0), std::string _tag = std::string())
		: Taggable(_tag)
		, location(_location)
		, color(_color)
		{
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<Taggable>(*this);
            ar & location;
            ar & color;
        }

		Vector  location;
		Color   color;
	};
}

#endif