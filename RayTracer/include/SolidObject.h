#ifndef _SOLID_OBJECT_H
#define _SOLID_OBJECT_H

#include "Optics.h"
#include "Taggable.h"
#include "Vector.h"
#include "Intersection.h"

namespace RayTracer
{
	class SolidObject: public Taggable
	{
	public:
		SolidObject(const Vector& _center = Vector(), bool _isFullyEnclosed = true)
			: center(_center)
			, refractiveIndex(REFRACTION_GLASS)
			, isFullyEnclosed(_isFullyEnclosed)
		{
		}

		virtual ~SolidObject()
		{
		}

		// Appends to 'intersectionList' all the intersections found starting at the specified vantage point in the direction of the direction vector.
		virtual void AppendAllIntersections(
			const Vector& vantage,
			const Vector& direction,
			std::vector<Intersection>& intersectionList) const = 0;

		/**
         * Searches for any intersections with this solid from the vantage point in the given direction.  
         * If none are found, the function returns 0 and the 'intersection' parameter is left unchanged.  
         * Otherwise, returns the positive number of intersections that lie at minimal distance from the vantage point in that direction.  
         * Usually this number will be 1 (a unique intersection is closer than all the others) but it can be greater if multiple intersections are 
         * equally close (e.g. the ray hitting exactly at the corner of a cube could cause this function to return 3).  
         * If this function returns a value greater than zero, it means the 'intersection' parameter has been filled in with the
		 * closest intersection (or one of the equally closest intersections).
		 */
        int FindClosestIntersection(
			const Vector& vantage,
			const Vector& direction,
			Intersection &intersection) const
		{
			cachedIntersectionList.clear();
			AppendAllIntersections(vantage, direction, cachedIntersectionList);
			return PickClosestIntersection(cachedIntersectionList, intersection);
		}

        /**
		 * Returns true if the given point is inside this solid object. This is a default implementation that counts intersections
		 * that enter or exit the solid in a given direction from the point.
		 * Derived classes can often implement a more efficient algorithm to override this default algorithm.
         */
		virtual bool Contains(const Vector& point) const;

        /**
		 * Returns the optical properties (reflection and refraction) at a given point on the surface of this solid.
		 * By default, the optical properties are the same everywhere,
		 * but a derived class may override this behavior to create patterns of different colors or gloss.
		 * It is recommended to keep constant refractive index throughout the solid, or the results may look weird.
		 */
        virtual Optics SurfaceOptics(
			const Vector& surfacePoint,
			const void *context) const
		{
			return uniformOptics;
		}

        /**
		 * Returns the index of refraction of this solid.  
		 * The refractive index is uniform throughout the solid.
		 */
        double GetRefractiveIndex() const
		{
			return refractiveIndex;
		}

        /**
		 * The following three member functions rotate this
		 * object counterclockwise around a line parallel
		 * to the x, y, or z axis, as seen from the positive
		 * axis direction.
         */
		virtual SolidObject& RotateX(double angleInDegrees) = 0;
		virtual SolidObject& RotateY(double angleInDegrees) = 0;
		virtual SolidObject& RotateZ(double angleInDegrees) = 0;

        /**
		 * Moves the entire solid object by the delta values dx, dy, dz.
		 * Derived classes that override this method must chain to it 
		 * in order to translate the center of rotation.
		 */
        virtual SolidObject& Translate(double dx, double dy, double dz)
		{
			center.x += dx;
			center.y += dy;
			center.z += dz;
			return *this;
		}

		// Moves the center of the solid object to the new location (cx, cy, cz).
		SolidObject& Move(double cx, double cy, double cz)
		{
			Translate(cx - center.x, cy - center.y, cz - center.z);
			return *this;
		}

		// Moves the center of the solid object to the location specified by the position vector newCenter.
		SolidObject& Move(const Vector& newCenter)
		{
			Move(newCenter.x, newCenter.y, newCenter.z);
			return *this;
		}

		const Vector& Center() const
		{
			return center;
		}

        /**
		 * Derived classes are allowed to report different optical
		 * properties at different points on their surface(s).
		 * For example, different points might have different matte
		 * colors in order to depict some kind of pattern on the solid.
		 * However, by default, solids have uniform optical properites
		 * everywhere on their surface(s).  This method allows callers
		 * that know a solid to have this default behavior to define
		 * the uniform optical properties.  All writes must take place
		 * before rendering starts in order to avoid weird/ugly results.
		 */
        void SetUniformOptics(const Optics& optics)
		{
			uniformOptics = optics;
		}

        /**
		 * Helper method for solids with uniform optics (see above).
		 * Defines the shiny reflection color (gloss) and dull reflection color (matte), 
         * with glossFactor = 0..1 that balances between the two modes of reflection.
		 * If glossFactor = 0, the object has a completely dull surface.  If glossFactor = 1, the surface is completely mirror-like.
		 */
        void SetMatteGlossBalance(
			double glossFactor,
			const Color& rawMatteColor,
			const Color& rawGlossColor)
		{
			uniformOptics.SetMatteGlossBalance(
				glossFactor,
				rawMatteColor,
				rawGlossColor);
		}

		void SetFullMatte(const Color& matteColor)
		{
			uniformOptics.SetMatteGlossBalance(
				0.0,                    // glossFactor=0 indicates full matte reflection
				matteColor,
				Color(0.0, 0.0, 0.0));  // irrelevant, but mass pass something
		}

		void SetOpacity(const double opacity)
		{
			uniformOptics.SetOpacity(opacity);
		}

		void SetRefraction(const double refraction)
		{
			ValidateRefraction(refraction);
			refractiveIndex = refraction;
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<Taggable>(*this);
            ar & center;
            ar & uniformOptics;
            ar & refractiveIndex;
            ar & const_cast<bool&>(isFullyEnclosed);
            ar & cachedIntersectionList;
            ar & enclosureList;
        }

	protected:

		const Optics& GetUniformOptics() const
		{
			return uniformOptics;
		}

	private:

        // The point in space about which this object rotates.
		Vector center;  

        /**
		 * By default, a solid object has uniform optical properties across its entire surface.  
         * Unless a derived class overrides the virtual member function SurfaceOptics(), the member variable uniformOptics holds these optical
		 * properties.
		 */
        Optics uniformOptics;

		// A solid object has a uniform refractive index throughout its contained volume.
		double refractiveIndex;

        /**
		 * A flag that indicates whether the Contains() method should try to determine whether a point is inside this solid.  
         * If true, containment calculations proceed; if false, Contains() always returns false.
		 * Many derived classes will override the Contains() method and therefore make this flag irrelevant.
		 */
        const bool isFullyEnclosed;

        /**
		 * The following members are an optimization to minimize the overhead and fragmentation caused by repeated 
		 * memory allocations creating and destroying std::vector contents.
		 */
        mutable std::vector<Intersection> cachedIntersectionList;
		mutable std::vector<Intersection> enclosureList;
	};
}

#endif