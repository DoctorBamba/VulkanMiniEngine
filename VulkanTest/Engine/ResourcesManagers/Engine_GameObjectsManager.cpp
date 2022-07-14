#include "Engine_GameObjectsManager.h"

namespace Engine
{
	ObjectsManager* objects_manager;
}

Uint Engine::ObjectsManager::NewObjectIdentefier()
{
	return gameobjects_allocator->AllocateElement();
}

Uint Engine::ObjectsManager::NewLightIdentefier()
{
	return lights_allocator->AllocateElement();
}

Uint Engine::ObjectsManager::NewArmatureRigion(Uint bones_count_)
{
	return bones_allocator->Allocate(bones_count_);
}