#pragma once
#include "../Scene/CMesh.h"

namespace Engine
{
	namespace Meshs
	{
		extern CMesh<CBaseVertex>* Quad2D;
		extern CMesh<CBaseVertex>* Cube;
		extern CMesh<CBaseVertex>* Sphere;
		extern CMesh<CBaseVertex>* Cylinder;
		extern CMesh<CBaseVertex>* Cone;
		extern CMesh<CBaseVertex>* Torus;
		extern CMesh<CBaseVertex>* PlanetControls;

		Void LoadBasicMeshs(CGpuUploadTask* upload_task_);
		CMesh<CBaseVertex>* NewControledCube(CGpuUploadTask* upload_task_, Uint subdevision_);
	}
}