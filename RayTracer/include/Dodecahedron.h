#ifndef _DODECAHEDRON_H_
#define _DODECAHEDRON_H_

#include "TriangleMesh.h"

namespace RayTracer
{
    // 12 faces, 20 edges
	class Dodecahedron: public TriangleMesh     
	{
	public:

        Dodecahedron(Vector center = Vector(), double scale = 0.0, const Optics& optics = Optics(Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.0), 1.0));

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<TriangleMesh>(*this);
        }

        virtual ~Dodecahedron() 
        { 
        }

    private:

		void AddFace(
			int aPointIndex, int bPointIndex, int cPointIndex, int dPointIndex, int ePointIndex,
			const Optics& optics,
			double edge);

		void CheckEdge(int aPointIndex, int bPointIndex, double edge) const;
	};
}

#endif