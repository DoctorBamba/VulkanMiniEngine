#include "Engine_Events.h"

Void Engine::Install()
{
	graphics			= new CGraphics();

	CreateBasicSamplers(graphics->p_Device);

	resource_manager	= new ResourcesMenager(graphics->p_Device, ENGINE_MAXIMUM_MATERIALS_COUNT, ENGINE_MAXIMUM_TEXTURES_COUNT);
	objects_manager		= new ObjectsManager(ENGINE_MAXIMUM_GAMEOBJECTS_COUNT, ENGINE_MAXIMUM_LIGHTS_COUNT, ENGINE_MAXIMUM_BONES_COUNT);
	renderer			= new Renderer();

	CGpuUploadTask* upload_task = new CGpuUploadTask(graphics->p_Device, graphics->p_CommandPool);
	upload_task->Reset();

	Meshs::LoadBasicMeshs(upload_task);

	Init_Event(upload_task);

	upload_task->Execute(graphics->p_GraphicsQueue);
	upload_task->WaitAntilComplite();

	delete upload_task;
}

Void Engine::Clean()
{
	if(graphics != nullptr)
		delete graphics;
}