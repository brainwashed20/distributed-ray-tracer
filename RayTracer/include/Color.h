#ifndef _COLOR_H
#define _COLOR_H

#include "stdafx.h"

namespace RayTracer
{
	class Color
	{
    public:
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & red;
            ar & green;
            ar & blue;
        }

		double  red;
		double  green;
		double  blue;
  
		Color(double _red, double _green, double _blue, double _luminosity = 1.0)
			: red(_luminosity * _red)
			, green(_luminosity * _green)
			, blue(_luminosity * _blue)
		{
		}

		Color()
			: red(0.0)
			, green(0.0)
			, blue(0.0)
		{
		}

		Color& operator += (const Color& other)
		{
			red += other.red;
			green += other.green;
			blue += other.blue;
			return *this;
		}

		Color& operator *= (const Color& other)
		{
			red *= other.red;
			green *= other.green;
			blue *= other.blue;
			return *this;
		}

		Color& operator *= (double factor)
		{
			red *= factor;
			green *= factor;
			blue *= factor;
			return *this;
		}

		Color& operator /= (double denom)
		{
			red /= denom;
			green /= denom;
			blue /= denom;
			return *this;
		}

		void Validate() const
		{
			if((red < 0.0) || (green < 0.0) || (blue < 0.0))
			{
				throw ImagerException("Negative color values not allowed.");
			}
		}
	};

	inline Color operator * (const Color& aColor, const Color& bColor)
	{
		return Color(
			aColor.red   * bColor.red,
			aColor.green * bColor.green,
			aColor.blue  * bColor.blue);
	}

	inline Color operator * (double scalar, const Color &color)
	{
		return Color(
			scalar * color.red,
			scalar * color.green,
			scalar * color.blue);
	}

	inline Color operator + (const Color& a, const Color& b)
	{
		return Color(
			a.red + b.red,
			a.green + b.green,
			a.blue + b.blue);
	}
}

#endif