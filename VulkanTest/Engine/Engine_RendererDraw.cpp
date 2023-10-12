#include "Engine_Renderer.h"
#include "Engine_BasicMeshs.h"

//Update Buffers...
Void Engine::Renderer::UpdateLookBuffer(CCamera* camera_, Uint frame_index_)
{
	frames_info.at(frame_index_)->look_ubuffer->Update(camera_->GetLookBuffer(), camera_->GetCameraId());

	auto pp_invocations = camera_->GetPostPocessingInvocations();

	for (auto pp_invocation_it = pp_invocations.begin(); pp_invocation_it != pp_invocations.end(); pp_invocation_it++)
		frames_info.at(frame_index_)->pp_invocation_ubuffer->Update(pp_invocation_it->properties_block, ENGINE_MAXIMUM_PP_INVOCS_PER_CAMERA_COUNT * camera_->GetCameraId());
};

Void Engine::Renderer::UpdateObjectsBuffers(CObject* object_, Uint frame_index_)
{
    //Update object's gpu buffer...
	Engine::GpuObjectStruct gpu_object_info;
	
	if (object_->GetMeshsNumber() > 0)
		gpu_object_info.material_index = Engine::resource_manager->GetMaterialLocation(object_->GetMesh(0)->GetMaterial());//Engine::materials_menager->GetBufferLocation(meshs.at(0)->GetMaterial());
	else
		gpu_object_info.material_index = INFINITE;

	CObject::ArmatureLink armature_link = object_->GetArmatureLink();

	if ((armature_link.armature != nullptr) && (armature_link.bones_internal_offset.size() > 0))
	{
		gpu_object_info.mesh_transform   = armature_link.armature->GetTransform();
		gpu_object_info.bones_ids_offset = armature_link.bones_internal_offset.at(0);
	}
	else
	{
		gpu_object_info.mesh_transform   = object_->GetTransform();
		gpu_object_info.bones_ids_offset = INFINITE;
	}

	frames_info.at(frame_index_)->objects_ubuffer->Update(gpu_object_info, object_->GetObjectId());

	//Update bones pose buffer(If this object is armature)...
	CArmature* armature_cast = dynamic_cast<CArmature*>(object_);
	if (armature_cast != nullptr)
	{
		UpdateBonesOffsetBuffer(armature_cast, frame_index_);
		UpdateBonesBuffer(armature_cast, frame_index_);
	}

	//Update childrens...
	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		UpdateObjectsBuffers(object_->GetChild(i), frame_index_);
}

Void Engine::Renderer::UpdateLightsBuffer(std::vector<CLight*> lights_, Uint frame_index_)
{
	for (Uint i = 0; i < lights_.size(); i++)
		frames_info.at(frame_index_)->lights_gbuffer->Update(lights_.at(i)->GetGpuStruct(), lights_.at(i)->GetLightId());
}

Void Engine::Renderer::UpdateBonesOffsetBuffer(CArmature* armature_, Uint frame_index_)
{
	Uint armature_offset = armature_->armature_offset;
	for (Uint i = 0; i < armature_->bones_poses.size(); i++)
		frames_info.at(frame_index_)->bones_offsets_ubuffer->Update(armature_->bones_offset.at(i), armature_offset + i);
}

Void Engine::Renderer::UpdateBonesBuffer(CArmature* armature_, Uint frame_index_)
{
	Uint armature_offset = armature_->armature_offset;
	for(Uint i = 0 ; i < armature_->bones_poses.size() ; i ++)
		frames_info.at(frame_index_)->bones_poses_ubuffer->Update(armature_->bones_poses.at(i), armature_offset + i);
}

Void Engine::Renderer::UpdateSceneBuffers(CScene* scene_, Uint frame_index_)
{
	UpdateLightsBuffer(scene_->GetAllLights(), frame_index_);
	UpdateObjectsBuffers(scene_->root_object, frame_index_);
}

Void Engine::Renderer::BindObject(CGpuDrawTask* draw_task_, CObject* object_)
{
	CArmature* armature = (CArmature*)object_->armature_link.armature;
	if (armature != nullptr)//If this object affectet by an armature
	{
		Uint offsets[3];
		offsets[0] = frames_info.at(draw_task_->GetFrameIndex())->objects_ubuffer->GetAligmentSize() * object_->GetObjectId();
		offsets[1] = frames_info.at(draw_task_->GetFrameIndex())->bones_poses_ubuffer->GetAligmentSize() * (armature->armature_offset + object_->armature_link.bones_internal_offset.at(0));
		offsets[2] = offsets[1];

		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetLayout(),
			2, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_object_descriptor_set), 3, offsets);
	}
	else
	{
		Uint offsets[3];
		offsets[0] = frames_info.at(draw_task_->GetFrameIndex())->objects_ubuffer->GetAligmentSize() * object_->GetObjectId();
		offsets[1] = 0;
		offsets[2] = 0;

		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetLayout(),
			2, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_object_descriptor_set), 3, offsets);
	}
}

//In propuse to save unnessecerly Pipeline bind calls we sort all visible objects into arrays base on the
//The next function sort Object hirarchy into shade instance base on the shade they use...
Void Engine::Renderer::SortObject(CObject* object_, Uint layer_)
{
	if (!(object_->visible_mask & (1 << layer_)))
		return;

	if (object_->GetMeshsNumber() > 0)//If this object has mesh
	{
		CMaterial* material = object_->GetMesh(0)->GetMaterial();
		if (material == nullptr)
			throw std::runtime_error("Renderer :: SortObject Error -> The mesh '" + object_->GetMesh(0)->GetName() + "' material undefine!!!");

		Shading* shade = shades->At(material->GetShadeId());
		shade->instances[shade->instances_count] = object_;
		shade->instances_count ++;
	}

	//Draw object's childrens...
	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		SortObject(object_->GetChild(i), layer_);
}

Void Engine::Renderer::SortScene(CScene* scene_, Uint layer_)
{
	//Clean...
	for (auto i = shades->Begin(); i != shades->End() ; i = shades->Next(i))
		i->value->instances_count = 0;

	//Sort...
	SortObject(scene_->root_object, layer_);
}

//Draw Object Hirarchy...
Void Engine::Renderer::DrawObject(CGpuDrawTask* draw_task_, CObject* object_, Uint layer_)
{
	if (!(object_->visible_mask & (1 << layer_)))
		return;

    if (object_->GetMeshsNumber() > 0)
    {
		BindObject(draw_task_, object_);
        object_->GetMesh(0)->Draw(draw_task_);
    }

    //Draw object's childrens...
    for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
        DrawObject(draw_task_, object_->GetChild(i), layer_);
}


Void Engine::Renderer::DrawScene(CGpuDrawTask* draw_task_, CScene* scene_, CCamera* camera_)
{
	//First step(Write To GBuffer)...
	
	frame_buffers.gbuffer->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::TARGET);
	frame_buffers.gbuffer->Open(draw_task_->GetCommandBuffer());

	for (auto i = shades->Begin(); i != shades->End(); i = shades->Next(i))
	{
		Shading* shade = i->value;
		if (shade->type != Shading::Type::Deffered)
			continue;

		shade->pipeline->Bind(draw_task_->GetCommandBuffer());

		for (Uint j = 0; j < shade->instances_count; j++)
			DrawObject(draw_task_, shade->instances[j], camera_->GetLayer());
	}

	frame_buffers.gbuffer->Close(draw_task_->GetCommandBuffer());

	//Secound step(Deferred Rendering)...
	BindPipeline(draw_task_->GetCommandBuffer(), "DeferredRendering");
	frame_buffers.gbuffer->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::RESOURCE);
	frame_buffers.canvas->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::TARGET);
	frame_buffers.canvas->Open(draw_task_->GetCommandBuffer());

	for (Uint i = 0; i < scene_->GetLightsCount() ; i ++)
	{
		Uint offset = frames_info.at(draw_task_->GetFrameIndex())->lights_gbuffer->GetAligmentSize() * scene_->GetLight(i)->GetLightId();
		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
			3, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_light_descriptor_set), 1, &offset);

		Meshs::Quad2D->Draw(draw_task_);//Draw frame quad
	}

	scene_->CallObjectsDrawEvent(draw_task_, camera_);

	for (auto i = shades->Begin(); i != shades->End(); i = shades->Next(i))
	{
		Shading* shade = i->value;
		if (shade->type != Shading::Type::Forward)
			continue;

		shade->pipeline->Bind(draw_task_->GetCommandBuffer());

		for (Uint j = 0; j < shade->instances_count; j++)
			DrawObject(draw_task_, shade->instances[j], camera_->GetLayer());
	}

	frame_buffers.canvas->Close(draw_task_->GetCommandBuffer());
	frame_buffers.canvas->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::RESOURCE);
}

//Draw Entire Scene...
Void Engine::Renderer::Draw(CGpuDrawTask* draw_task_, CScene* scene_)
{
	//Sort scene object...
	SortScene(scene_);

	//Update scene buffers
	UpdateSceneBuffers(scene_, draw_task_->GetFrameIndex());

	//Bind static descriptor set...
	VkDescriptorSet static_set = resource_manager->GetDescriptorSet();
	vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
		0, 1, &static_set, 0, nullptr);
	
	//Draw scene into all cameras...
	Uint offsets[2] = { 0, 0 };
	Uint& camera_offset = offsets[0];


	for (std::map<Float, CCamera*>::iterator it = scene_->GetAllCameras()->begin() ; it != scene_->GetAllCameras()->end() ; it ++)
	{
		CCamera* camera = it->second;
		camera_offset = camera->GetCameraId();

		if (!bounded_cameras[camera_offset].bounded && camera->GetFrameBuffer()->IsBoundable())
		{
			Engine::resource_manager->AddTexturesPacket({ camera->GetFrameBuffer()->GetDepthStencilSurface(), camera->GetFrameBuffer()->GetColorSurface(0) });
			bounded_cameras[camera_offset].depth_loc	= Engine::resource_manager->GetTextureLocation(camera->GetFrameBuffer()->GetDepthStencilSurface());
			bounded_cameras[camera_offset].color_loc[0] = Engine::resource_manager->GetTextureLocation(camera->GetFrameBuffer()->GetColorSurface(0));
		
			bounded_cameras[camera_offset].bounded = true;
		}

		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
			1, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_frame_descriptor_set), 2, offsets);
		
		DrawScene(draw_task_, scene_, camera);

		UpdateLookBuffer(camera, draw_task_->GetFrameIndex());/*Importent Note! : All of the recent recorded draw commands only will only be performed in the future so you might
		assume that its not importent whether the calling to this buffer update comes befor or after the 'DrawScene' call, But for certain reasons I prefer it to be possible
		to add a post-processing-invocation from the gameobject's 'draw' call, And that why i call this function after 'DrawScene' call!*/
	

		//Third step(Post processing stage)...
		//In the post processing stage we have tow regular rgba-frame-buffers one.
		//In each post processing call we write into one of those frame buffer while 
		//the other one can used has a shader resource.
		//At the last post processing call we open the output target instad to write 
		//into the window or render-port target buffer.
		//In the post processing stage you allow to bind input shader data throw the per-frame binding set layout at location 16.
	

		Uint offsets[2] = { 0, 0 };
		Uint target_index, source_index;
		Int invocation_counter = 0;
		
		auto pp_invocations = camera->GetPostPocessingInvocations();
		
		for (auto pp_invocation_it = pp_invocations.begin() ; pp_invocation_it != pp_invocations.end() ; pp_invocation_it++)
		{
			offsets[1] = ENGINE_MAXIMUM_PP_INVOCS_PER_CAMERA_COUNT * camera->GetCameraId() + invocation_counter;

			camera->GetFrameBuffer()->Open(draw_task_->GetCommandBuffer());
			pp_invocation_it->pipeline->Bind(draw_task_->GetCommandBuffer());

			vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
				DescriptorSetLayouts::PerCamera, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_frame_descriptor_set), 2, offsets);
			

			Meshs::Quad2D->Draw(draw_task_);//Draw frame quad

			camera->GetFrameBuffer()->Close(draw_task_->GetCommandBuffer());
			invocation_counter++;
		}

		camera->ResetPostProcessingInvocation();
	}
}