#include "Planet.h"
#include "ThinRing.h"

namespace RayTracer
{
	SolidObject* Saturn::CreateRingSystem()
	{
		struct RingData
		{
			double  innerRadiusKm;
			double  outerRadiusKm;
			double  red;
			double  green;
			double  blue;
		};

		static const RingData ringData[] =
		{
			{92154.0, 117733.0, 196.0, 194.0, 180.0},
			{122405.0, 133501.0, 157.0, 160.0, 158.0},
			{134085.0, 136888.0, 136.0, 140.0, 142.0}
		};

		SolidObject* ringSystem = nullptr;

		static const size_t NUM_RINGS = sizeof(ringData) / sizeof(ringData[0]);
		for(size_t i = 0; i < NUM_RINGS; ++i)
		{
			const Color color(ringData[i].red / 255.0, ringData[i].green / 255.0, ringData[i].blue / 255.0);

			ThinRing* ringSolid = new ThinRing(ringData[i].innerRadiusKm / MEAN_EARTH_RADIUS_KM, ringData[i].outerRadiusKm / MEAN_EARTH_RADIUS_KM);
			ringSolid->SetFullMatte(color);

			if(ringSystem)
			{
				ringSystem = new SetUnion(Vector(), ringSolid, ringSystem);
			}
			else
			{
				ringSystem = ringSolid;
			}
		}

		return ringSystem;
	}
}
