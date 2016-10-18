#ifndef _OPTICS_H_
#define _OPTICS_H_

#include "Color.h"

namespace RayTracer
{
	// class Optics describes the way a surface point interacts with light.
	class Optics
	{
	public:

		explicit Optics(
            Color _matteColor = Color(1.0, 1.0, 1.0),
			Color _glossColor = Color(0.0, 0.0, 0.0),
			double _opacity = 1.0)
		{
			SetMatteColor(_matteColor);
			SetGlossColor(_glossColor);
			SetOpacity(_opacity);
		}

		void SetMatteGlossBalance(
			double glossFactor,     // 0..1: balance between matte and gloss
			const Color& rawMatteColor,
			const Color& rawGlossColor);

		void SetMatteColor(const Color& _matteColor);
		void SetGlossColor(const Color& _glossColor);
		void SetOpacity(double _opacity);

		const Color& GetMatteColor() const
		{
			return matteColor;
		}

		const Color& GetGlossColor() const
		{
			return glossColor;
		}
		
        const double GetOpacity()    const
		{
			return opacity;
		}

        friend class access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & matteColor;
            ar & glossColor;
            ar & opacity;
        }

	protected:
	
        void ValidateReflectionColor(const Color& color) const;

	private:
    
		Color   matteColor;     // color, intensity of scattered reflection
		Color   glossColor;     // color, intensity of mirror reflection
		double  opacity;        // fraction 0..1 of reflected light
	};
}

#endif