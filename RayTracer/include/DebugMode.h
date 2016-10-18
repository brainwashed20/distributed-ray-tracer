#ifndef _DEBUG_MODE_H_
#define _DEBUG_MODE_H_

#include "Color.h"
#include "Intersection.h"

namespace RayTracer
{
	void Indent(std::ostream& output, int depth)
	{
		const int numSpaces = 4 * depth;
		for(int i = 0; i < numSpaces; ++i)
		{
			output << ' ';
		}
	}

	std::ostream& operator<< (std::ostream &output, const Color& color)
	{
		using namespace std;

		output.precision(3);
		output << scientific;
		output << "(R " << color.red << ", G " << color.green << ", B " << color.blue << ")";
		return output;
	}

	std::ostream& operator<< (std::ostream& output, const Intersection& intersection)
	{
		using namespace std;

		output << "{ ";
		if(intersection.solid)
		{
			output << intersection.solid->GetTag();
		}
		if(intersection.tag)
		{
			output << ":" << intersection.tag;
		}
		output.precision(3);
		output << fixed;
		output << " d=" << intersection.distanceSquared;
		output << ", p=" << intersection.point;
		output << ", n=" << intersection.surfaceNormal;
		output << " }";
		return output;
	}

	std::ostream& operator<< (std::ostream& output, const Vector& vector)
	{
		using namespace std;

		output.precision(3);
		output << fixed;
		output << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
		return output;
	}
}

#endif