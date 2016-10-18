#ifndef _ICOSAHEDRON_H_
#define _ICOSAHEDRON_H_

#include "TriangleMesh.h"

namespace RayTracer
{
    // 20 faces, 12 edges
	class Icosahedron: public TriangleMesh     
	{
	public:
        Icosahedron(Vector center = Vector(), double scale = 0.0, const Optics& optics = Optics(Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 0.0), 1.0));

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<TriangleMesh>(*this);
        }
	};
}

#endif