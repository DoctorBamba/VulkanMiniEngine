#pragma once
#include "CPlanetSurface.h"
#include "CPlanetAtmosphere.h"
#include "CPlanetOcean.h"

class CPlanet : public CObject
{
	public:
		CPlanetSurface*		surface;
		CPlanetOcean*		ocean;
		CPlanetAtmosphere*	atmosphere;

		CPlanet()
		{
			surface = new CPlanetSurface();
			AddChild(surface);

			ocean = new CPlanetOcean();
			AddChild(ocean);

			atmosphere = new CPlanetAtmosphere(140.0f);
			AddChild(atmosphere);
		}
};

