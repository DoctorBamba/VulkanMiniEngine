#include "Engine_Renderer.h"
#include "Engine_BasicMeshs.h"

//Update Buffers...
Void Engine::Renderer::UpdateLookBuffer(CCamera* camera_, Uint layout_, Uint frame_index_)
{
	if (layout_ >= layers_cameras.size())
		throw std::range_error("Renderer :: UpdateLookBuffer Error -> layout must be lower then the number to layers!");

	frames_info.at(frame_index_)->look_ubuffer->Update(camera_->GetLookBuffer(), layout_);
	layers_cameras.at(layout_) = camera_;
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

//In propuse to save unnessecerly Pipeline bind calls the we sort all visible objects into arrays base on the
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

		Shade* shade = shades->At(material->GetShadeId());
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
        object_->GetMesh(0)->Render(draw_task_);
    }

    //Draw object's childrens...
    for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
        DrawObject(draw_task_, object_->GetChild(i), layer_);
}


Void Engine::Renderer::DrawScene(CGpuDrawTask* draw_task_, CScene* scene_, CFrameBuffer* output_, Uint layer_)
{
	//First step(Write To GBuffer)...
	
	frame_buffers.gbuffer->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::TARGET);
	frame_buffers.gbuffer->Open(draw_task_->GetCommandBuffer());

	for (auto i = shades->Begin(); i != shades->End(); i = shades->Next(i))
	{
		Shade* shade = i->value;
		if (shade->type != Shade::Type::Deffered)
			continue;

		shade->pipeline->Bind(draw_task_->GetCommandBuffer());

		for (Uint j = 0; j < shade->instances_count; j++)
			DrawObject(draw_task_, shade->instances[j], layer_);
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

		Meshs::Quad2D->Render(draw_task_);//Draw frame quad
	}

	scene_->CallObjectsDrawEvent(draw_task_, layer_);

	for (auto i = shades->Begin(); i != shades->End(); i = shades->Next(i))
	{
		Shade* shade = i->value;
		if (shade->type != Shade::Type::Forward)
			continue;

		shade->pipeline->Bind(draw_task_->GetCommandBuffer());

		for (Uint j = 0; j < shade->instances_count; j++)
			DrawObject(draw_task_, shade->instances[j], layer_);
	}

	frame_buffers.canvas->Close(draw_task_->GetCommandBuffer());
	frame_buffers.canvas->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::RESOURCE);

	//Third step(Post processing stage)...
	//In the post processing stage we have tow regular rgba-frame-buffers one.
	//In each post processing call we write into one of those frame buffer while 
	//the other one can used has a shader resource.
	//At the last post processing call we open the output target instad to write 
	//into the window or render-port target buffer.
	//In the post processing stage you allow to bind input shader data throw the per-frame binding set layout at location 16.
	
	Uint offsets[2] = { 0, 0 };
	Uint target_index, source_index;
	Int i = 0;
	
	if(post_processings_calls.size() > 0)
	{//Last post-processing call...
		source_index = post_processings_calls.size() % 2;

		output_->Open(draw_task_->GetCommandBuffer());
		post_processings_calls.at(i)->Bind(draw_task_->GetCommandBuffer());
		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
			1, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_frame_descriptor_set), 2, offsets);
		vkCmdPushConstants(draw_task_->GetCommandBuffer(), layout, VK_SHADER_STAGE_ALL, 0, sizeof(Uint), &source_index);
		Meshs::Quad2D->Render(draw_task_);//Draw frame quad
		output_->Close(draw_task_->GetCommandBuffer());

		offsets[1] += sizeof(GpuPropertiesBlockStruct);
	}

	post_processings_calls.clear();//Clear the post processing call for the next frame recording
}

//Draw Entire Scene...
Void Engine::Renderer::Render(CGpuDrawTask* draw_task_, CFrameBuffer* output_, CScene* scene_)
{
	//Sort scene object...
	SortScene(scene_);

	//Bind static descriptor set
	VkDescriptorSet static_set = resource_manager->GetDescriptorSet();
	vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
		0, 1, &static_set, 0, nullptr);
	
	//Offsets...
	const Uint camera_ubuffer_size = frames_info.at(draw_task_->GetFrameIndex())->look_ubuffer->GetAligmentSize();
	Uint offsets[2] = { 0, 0 };
	
	for (Uint i = 1 ; i < frame_buffers.layers.size() ; i ++)
	{
		offsets[0] += camera_ubuffer_size;
		vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
			1, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_frame_descriptor_set), 2, offsets);
		
		frame_buffers.layers.at(i)->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::TARGET);
		DrawScene(draw_task_, scene_, frame_buffers.layers.at(i), (Uint)DrawLayers::Reflections);
		frame_buffers.layers.at(i)->Barrier(draw_task_->GetCommandBuffer(), CFrameBuffer::BarrierState::RESOURCE);
	}

	offsets[0] = 0;
	vkCmdBindDescriptorSets(draw_task_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLayout(),
		1, 1, &(frames_info.at(draw_task_->GetFrameIndex())->per_frame_descriptor_set), 2, offsets);
	
	DrawScene(draw_task_, scene_, output_, (Uint)DrawLayers::Base);
}

Uint Engine::Renderer::AddNewLayer()
{
	layers_cameras.push_back(nullptr);
	return layers_cameras.size() - 1;
}

Void Engine::Renderer::AddPostProcessingCall(CPipeline* pipeline_, const GpuPropertiesBlockStruct& shader_input_, Uint frame_index_)
{
	frames_info.at(frame_index_)->postcall_ubuffer->Update(shader_input_, post_processings_calls.size());
	post_processings_calls.push_back(pipeline_);
}