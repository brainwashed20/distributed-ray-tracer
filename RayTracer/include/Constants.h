#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include "ImagerException.h"

namespace RayTracer
{
	const double PI = 3.141592653589793238462643383279502884;
    
    /**
	 * EPSILON is a tolerance value for floating point roundoff error.
	 * It is used in many calculations where we want to err
	 * on a certain side of a threshold, such as determining
	 * whether or not a point is inside a solid or not,
	 * or whether a point is at least a minimum distance
	 * away from another point.
	 */
    const double EPSILON = 1.0e-6;

	//  Refractive indexes of common substances/media...
	const double REFRACTION_VACUUM = 1.0000;
	const double REFRACTION_AIR = 1.0003;
	const double REFRACTION_ICE = 1.3100;
	const double REFRACTION_WATER = 1.3330;
	const double REFRACTION_GASOLINE = 1.3980;
	const double REFRACTION_GLASS = 1.5500;
	const double REFRACTION_SAPPHIRE = 1.7700;
	const double REFRACTION_DIAMOND = 2.4190;

	// Range of allowed refraction values...
	const double REFRACTION_MINIMUM = 1.0000;
	const double REFRACTION_MAXIMUM = 9.0000;

	inline double RadiansFromDegrees(double degrees)
	{
		return degrees * (PI / 180.0);
	}

	inline void ValidateRefraction(double refraction)
	{
		if(refraction < REFRACTION_MINIMUM 
        || refraction > REFRACTION_MAXIMUM)
		{
			throw ImagerException("Invalid refractive index.");
		}
	}
}

#endif