#ifndef _PLANET_H_
#define _PLANET_H_

#include "Spheroid.h"
#include "SetUnion.h"

namespace RayTracer
{
	const double MEAN_EARTH_RADIUS_KM = 6371.0;

	class Planet: public Spheroid
	{
	public:

        Planet(
            Color _color = Color(0.0, 0.0, 0.0),
			double _equatorialRadiusInKm = 0.0,
			double _polarRadiusInKm = 0.0)
			: Spheroid(
			    _equatorialRadiusInKm / MEAN_EARTH_RADIUS_KM,
			    _equatorialRadiusInKm / MEAN_EARTH_RADIUS_KM,
			    _polarRadiusInKm / MEAN_EARTH_RADIUS_KM)
		{
			SetFullMatte(_color);
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<Spheroid>(*this);
        }
	};

	class Saturn: public SetUnion
	{
	public:

		Saturn()
			: SetUnion(Vector(), new Planet(Color(1.0, 1.0, 0.6, 0.335), 60268.0, 54364.0), CreateRingSystem())
		{
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SetUnion>(*this);
        }


	private:
      
		static SolidObject* CreateRingSystem();
	};
}

#endif