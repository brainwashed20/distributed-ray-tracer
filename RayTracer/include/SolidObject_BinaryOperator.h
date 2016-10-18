#ifndef _SOLIDOBJECT_BINARYOPERATOR_H_
#define _SOLIDOBJECT_BINARYOPERATOR_H_

#include "SolidObject.h"

namespace RayTracer
{
	/**
     * This class encapsulates the notion of a binary operator that operates on two SolidObjects.  
     * Both SolidObjects  must support the Contains() method, or an exception will occur during rendering.
	 */
    class SolidObject_BinaryOperator: public SolidObject
	{
	public:

        /**
		 * The parameters '_left' and '_right' must be dynamically  allocated using operator new. 
         * This class will own responsibility for deleting them when it is itself deleted.
		 */
        SolidObject_BinaryOperator(
			const Vector& _center = Vector(),
			SolidObject* _left = nullptr,
			SolidObject* _right = nullptr)
			: SolidObject(_center)
			, left(_left)
			, right(_right)
		{
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<SolidObject>(*this);
            ar & left;
            ar & right;
        }

		virtual ~SolidObject_BinaryOperator()
		{
			delete left;
			left = NULL;

			delete right;
			right = NULL;
		}

        /**
		 * All rotations and translations are applied to the two nested solids in tandem.
		 * The following three member functions rotate this object counterclockwise around a line parallel 
         * to the x, y, or z axis, as seen from the positive axis direction.
		 */
        virtual SolidObject& RotateX(double angleInDegrees);
		virtual SolidObject& RotateY(double angleInDegrees);
		virtual SolidObject& RotateZ(double angleInDegrees);

		virtual SolidObject& Translate(double dx, double dy, double dz);

	protected:

		SolidObject& Left()  const
		{
			return *left;
		}
		SolidObject& Right() const
		{
			return *right;
		}

		void NestedRotateX(
			SolidObject &nested,
			double angleInDegrees,
			double a,
			double b);

		void NestedRotateY(
			SolidObject &nested,
			double angleInDegrees,
			double a,
			double b);

		void NestedRotateZ(
			SolidObject &nested,
			double angleInDegrees,
			double a,
			double b);

		/**
         * The following list is for caching and filtering  intersections with the left and right nested solids.
		 * It is mutable to allow modification from const methods.
		 */
        mutable std::vector<Intersection> tempIntersectionList;

	private:
       
		SolidObject* left;
		SolidObject* right;
	};
}

#endif