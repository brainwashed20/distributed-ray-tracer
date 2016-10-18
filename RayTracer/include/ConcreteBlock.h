#ifndef _CONCRETE_BLOCK_H_
#define _CONCRETE_BLOCK_H_

#include "SetDifference.h"
#include "SetUnion.h"
#include "Cuboid.h"

namespace RayTracer
{
    /**
	 * A ConcreteBlock is a large cuboid with the union of two smaller cuboids subtracted from it.
	 * ConcreteBlock = SetDifference(LargeCuboid, SetUnion(SmallCuboid1, SmallCuboid2))
     */
	class ConcreteBlock: public SetDifference
	{
	public:
        ConcreteBlock(const Vector& _center = Vector(), const Optics& _optics = Optics(Color(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.0), 1.0))
			: SetDifference(Vector(), CreateLargeCuboid(_optics), CreateSmallCuboidUnion(_optics))
		{
			Move(_center);
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SetDifference>(*this);
        }

	private:

		static SolidObject* CreateLargeCuboid(const Optics& _optics)
		{
			Cuboid* cuboid = new Cuboid(8.0, 16.0, 8.0);
			cuboid->SetUniformOptics(_optics);
			return cuboid;
		}

		static SolidObject* CreateSmallCuboidUnion(const Optics& _optics)
		{
			// We add a little bit in the z dimension (8.01 instead of 8.0) to guarantee completely engulfing the hole.

			Cuboid *topCuboid = new Cuboid(6.0, 6.5, 8.01);
			Cuboid *bottomCuboid = new Cuboid(6.0, 6.5, 8.01);

			topCuboid->SetUniformOptics(_optics);
			bottomCuboid->SetUniformOptics(_optics);

			topCuboid->Move(0.0, +7.5, 0.0);
			bottomCuboid->Move(0.0, -7.5, 0.0);

			return new SetUnion(Vector(), topCuboid, bottomCuboid);
		}
	};
}

#endif