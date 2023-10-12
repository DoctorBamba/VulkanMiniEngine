#pragma once
#include "../../Graphics/CAllocator.h"

#define ENGINE_MAXIMUM_VIEWS_COUNT						16
#define ENGINE_MAXIMUM_GAMEOBJECTS_COUNT				100000
#define ENGINE_MAXIMUM_LIGHTS_COUNT						30000
#define ENGINE_MAXIMUM_BONES_COUNT						100000
#define ENGINE_MAXIMUM_CAMERAS_COUNT					256
#define ENGINE_MAXIMUM_PP_INVOCS_PER_CAMERA_COUNT		16
#define ENGINE_MAXIMUM_PP_INVOCS_COUNT					(ENGINE_MAXIMUM_CAMERAS_COUNT * ENGINE_MAXIMUM_PP_INVOCS_PER_CAMERA_COUNT)

class CObject;
class CCamera;
class CLight;
class CArmature;

namespace Engine
{
	class ObjectsManager
	{
		friend CObject;
		friend CCamera;
		friend CLight;
		friend CArmature;


		private:
			CBasicAllocator*  gameobjects_allocator;
			CBasicAllocator*  cameras_allocator;
			CBasicAllocator*  lights_allocator;
			CBlocksAllocator* bones_allocator;

			Uint NewObjectIdentefier();
			Uint NewCameraIdentefier();
			Uint NewLightIdentefier();
			Uint NewArmatureRigion(Uint bones_count_);

		public:

			ObjectsManager(Uint maximum_objects_count_, Uint maximum_cameras_count, Uint maximum_lights_count_, Uint maximum_bones_count_)
			{
				gameobjects_allocator	= new CBasicAllocator(maximum_objects_count_);
				cameras_allocator		= new CBasicAllocator(maximum_cameras_count);
				lights_allocator		= new CBasicAllocator(maximum_lights_count_);
				bones_allocator			= new CBlocksAllocator(maximum_bones_count_);
			}
	};


	extern ObjectsManager* objects_manager;
}