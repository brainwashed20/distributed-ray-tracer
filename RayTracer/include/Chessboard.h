#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "Cuboid.h"

namespace RayTracer
{
	class ChessBoard: public Cuboid
	{
	public:
		ChessBoard(
			double _innerSize = 0.0,
			double _xBorderSize = 0.0,
			double _yBorderSize = 0.0,
			double _thickness = 0.0,
			const Color& _lightSquareColor = Color(0.0, 0.0, 0.0),
            const Color& _darkSquareColor = Color(0.0, 0.0, 0.0),
            const Color& _borderColor = Color(0.0, 0.0, 0.0));

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<Cuboid>(*this);
            ar & innerSize;
            ar & xBorderSize;
            ar & yBorderSize;
            ar & thickness;
            ar & lightSquareColor;
            ar & darkSquareColor;
            ar & borderColor;
        }

	protected:
        /**
		 * This method override provides the variety of colors
		 * for the light squares, dark squares, and border.
		 */
        virtual Optics ObjectSpace_SurfaceOptics(const Vector& surfacePoint, const void *context) const;

		int SquareCoordinate(double xy) const;

	private:

		double innerSize;
		double xBorderSize;
		double yBorderSize;
		double thickness;
		Color  lightSquareColor;
		Color  darkSquareColor;
		Color  borderColor;
	};
}

#endif