#include "Engine_Renderer.h"

namespace Engine
{
	Renderer* renderer;
}

Void Engine::Renderer::CreateShade(std::string name_, PipelineState pipeline_info_, Shading::Type shade_type_, Bool present_)
{
	auto it = shades_ids.find(name_);
	if (it != shades_ids.end())
	{
		throw std::runtime_error("Renderer :: CreateShade -> An other shade have the given name.");
		return;
	}

	pipeline_info_.input.inputLayout = this->layout;

	Shading* shade = new Shading(shade_type_, new CPipeline(graphics->p_Device->device, name_, pipeline_info_, present_), name_);
	shades_ids.insert(std::pair<std::string, Uint>(name_, shades->PushBack(shade)));

}

Void Engine::Renderer::BindPipeline(VkCommandBuffer command_buffer_, std::string pipeline_name_)
{
	auto it = shades_ids.find(pipeline_name_);
	if (it == shades_ids.end())
	{
		throw std::runtime_error("BindPipeline Error -> Not pipeline with name '" + pipeline_name_ + "' is found");
		return;
	}

	shades->At(it->second)->pipeline->Bind(command_buffer_);
}

//The next function define the pipelines layout/root-signature that the renderer use
Void Engine::Renderer::CreateLayout()
{
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_desc{};

	//Static descriptor set..
	descriptor_set_layouts[(Uint)DescriptorSetLayouts::PerResourceManager] = Engine::resource_manager->GetDescriptorLayout();

	//Per frame descriptor set...

	VkDescriptorSetLayoutBinding per_frame_binds[24];

	VkDescriptorSetLayoutBinding& camera_binding = per_frame_binds[0];
	camera_binding = {};
	camera_binding.binding					= 0;
	camera_binding.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	camera_binding.descriptorCount			= 1;
	camera_binding.stageFlags				= VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutBinding& general_binding = per_frame_binds[1];
	general_binding = {};
	general_binding.binding					= 1;
	general_binding.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	general_binding.descriptorCount			= 1;
	general_binding.stageFlags				= VK_SHADER_STAGE_FRAGMENT_BIT;

	for (Uint i = 2 ; i < 12; i ++)
	{
		VkDescriptorSetLayoutBinding& gbuffer_binding = per_frame_binds[i];
		gbuffer_binding = {};
		gbuffer_binding.binding			= i;
		gbuffer_binding.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		gbuffer_binding.descriptorCount = 1;
		gbuffer_binding.stageFlags		= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	for (Uint i = 12 ; i < 24; i ++)
	{
		VkDescriptorSetLayoutBinding& reflection_binding = per_frame_binds[i];
		reflection_binding = {};
		reflection_binding.binding			= i;
		reflection_binding.descriptorType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		reflection_binding.descriptorCount	= 1;
		reflection_binding.stageFlags		= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(per_frame_binds);
	descriptor_set_layout_desc.pBindings	= per_frame_binds;

	if (vkCreateDescriptorSetLayout(Engine::graphics->GetVkDevice(), &descriptor_set_layout_desc, nullptr, &descriptor_set_layouts[DescriptorSetLayouts::PerCamera]) != VK_SUCCESS)
	{
		throw std::runtime_error("Renderer::CreateLayout -> Failed to create descriptor set layout!");
		return;
	}

	//Per Object Descriptors Set...

	VkDescriptorSetLayoutBinding object_info_binding{};
	object_info_binding.binding				= 0;
	object_info_binding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	object_info_binding.descriptorCount		= 1;
	object_info_binding.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bones_offsets_binding{};
	bones_offsets_binding.binding			= 1;
	bones_offsets_binding.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	bones_offsets_binding.descriptorCount	= 1;
	bones_offsets_binding.stageFlags		= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	VkDescriptorSetLayoutBinding bones_poses_binding{};
	bones_poses_binding.binding				= 2;
	bones_poses_binding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	bones_poses_binding.descriptorCount		= 1;
	bones_poses_binding.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	VkDescriptorSetLayoutBinding per_object_binds[] = { object_info_binding, bones_offsets_binding, bones_poses_binding };

	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(per_object_binds);
	descriptor_set_layout_desc.pBindings	= per_object_binds;

	if (vkCreateDescriptorSetLayout(Engine::graphics->GetVkDevice(), &descriptor_set_layout_desc, nullptr, &descriptor_set_layouts[DescriptorSetLayouts::PerGameObject]) != VK_SUCCESS)
	{
		throw std::runtime_error("Renderer::CreateLayout -> Failed to create descriptor set layout!");
		return;
	}

	//Per light descriptor block...

	VkDescriptorSetLayoutBinding light_binding{};
	light_binding.binding					= 0;
	light_binding.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	light_binding.descriptorCount			= 1;
	light_binding.stageFlags				= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding shadowmaps_binding{};
	shadowmaps_binding.binding				= 1;
	shadowmaps_binding.descriptorType		= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	shadowmaps_binding.descriptorCount		= 1;
	shadowmaps_binding.stageFlags			= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding per_light_binds[] = { light_binding };
	
	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(per_light_binds);
	descriptor_set_layout_desc.pBindings	= per_light_binds;

	if (vkCreateDescriptorSetLayout(graphics->GetVkDevice(), &descriptor_set_layout_desc, nullptr, &descriptor_set_layouts[DescriptorSetLayouts::PerLight]) != VK_SUCCESS)
	{
		throw std::runtime_error("Renderer::CreateLayout -> Failed to create descriptor set layout!");
		return;
	}

	//Per Cmd Constants...

	VkPushConstantRange push_constat_range;
	push_constat_range.offset		= 0;
	push_constat_range.size			= sizeof(Uint);
	push_constat_range.stageFlags	= VK_SHADER_STAGE_ALL;

	//Pipeline Layout...

	VkPipelineLayout pipe_line_layout;
	VkPipelineLayoutCreateInfo pipeline_layout_desc{};
	pipeline_layout_desc.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_desc.setLayoutCount				= DescriptorSetLayouts::COUNT;
	pipeline_layout_desc.pSetLayouts				= descriptor_set_layouts;
	pipeline_layout_desc.pushConstantRangeCount		= 1;
	pipeline_layout_desc.pPushConstantRanges		= &push_constat_range;

	if (vkCreatePipelineLayout(graphics->GetVkDevice(), &pipeline_layout_desc, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("Renderer::CreateLayout -> Failed to create pipeline layout!");
		return;
	}
}

Void Engine::Renderer::InitialStatics(CVulkanDevice* device)
{
	//Static Render Pass...

	RenderPass::forward  = new CRenderPass(device->device, { VK_FORMAT_B8G8R8A8_UNORM }, VK_FORMAT_D32_SFLOAT, RenderPassType::Texture2D);
	RenderPass::postproc = new CRenderPass(device->device, { VK_FORMAT_B8G8R8A8_UNORM }, VK_FORMAT_UNDEFINED, RenderPassType::Texture2D);
	RenderPass::output	 = new CRenderPass(device->device, { VK_FORMAT_B8G8R8A8_UNORM }, VK_FORMAT_UNDEFINED, RenderPassType::Present);

	RenderPass::deferred = new CRenderPass(graphics->GetVkDevice(),
												  { VK_FORMAT_R8G8B8A8_UNORM,
													VK_FORMAT_R32G32_UINT,
													VK_FORMAT_R32G32_UINT,
													VK_FORMAT_R8G8B8A8_UNORM},
													VK_FORMAT_D32_SFLOAT,
													RenderPassType::Texture2D);
}

Void Engine::Renderer::CreateBasicGraphicsPipelines()
{
	//Create the "Write-To-GBuffer" Pipeline...


	InputAttributes vertex_attributes = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },			//Position
									  {0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },			//Texcoord
									  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },			//Normal
									  {0, 3, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },			//Tangent
									  {0, 4, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },			//Bitangent
									  {0, 5, VK_FORMAT_R32G32B32A32_UINT, VK_VERTEX_INPUT_RATE_VERTEX },		//Bones Ids
									  {0, 6, VK_FORMAT_R32G32B32A32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };	//Bones Wigths

	PipelineState pipeline_info{};
	pipeline_info.input.topology			= PrimitiveTopology::Triangles;
	pipeline_info.input.vertexAttributes	= vertex_attributes;
	pipeline_info.vertexStage				= CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/GBufferWrite.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.fargmentStage				= CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/GBufferWrite.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.rasterization.faceCulling = FaceCulling::Front;
	pipeline_info.rasterization.polygonMode = PolygonMode::Fill;
	pipeline_info.output.blendingStates		= BlendingStatesDeclarationParser::Parse("Blend Add One Zero");
	pipeline_info.output.depthState			= AttachmentDepthState(true, true, ComparisonOperator::Less);
	pipeline_info.output.existingRenderPass	= RenderPass::deferred;

	CreateShade("BasicPbr", pipeline_info, Shading::Type::Deffered, false);

	//Create the "Deferred-Rendering" Pipeline...

	InputAttributes framequad_vertex_attributes = {{0, 0, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },		//Position
										{0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX }};		//Bitangent

	pipeline_info = {};
	pipeline_info.input.topology			= PrimitiveTopology::Triangles;
	pipeline_info.input.vertexAttributes	= framequad_vertex_attributes;
	pipeline_info.vertexStage				= CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Framequad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.fargmentStage				= CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Deferred.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.rasterization.faceCulling = FaceCulling::Front;
	pipeline_info.rasterization.polygonMode = PolygonMode::Fill;
	pipeline_info.output.blendingStates		= BlendingStatesDeclarationParser::Parse("Blend Add One One");
	pipeline_info.output.depthState			= AttachmentDepthState(true, true, ComparisonOperator::Less);
	pipeline_info.output.existingRenderPass = RenderPass::forward;

	CreateShade("DeferredRendering", pipeline_info, Shading::Type::Forward, false);
	
}


Void Engine::Renderer::Resize(Uint16 width_, Uint16 height_)
{
	extent = Extent(width_, height_);

	auto it = avalible_extents.find(extent);
	if (it == avalible_extents.end())
	{
		CMemorySpace* const local_space = graphics->p_Device->MemorySpaces.local_space;

		//Create a new the Frame Buffer correspoding to the new extent...
		CTextureBase*			   gbuffers_depth_surface;//For some effect we want to access the depth in a regulat shade material so we need saperate depth resource
		std::vector<CTextureBase*> gbuffers_surfaces;
		CTextureBase*			   depth_surface;
		CTextureBase*			   postprocs_surfaces[2];

		gbuffers_depth_surface	  = new CTexture2D(local_space, width_, height_, VK_FORMAT_D32_SFLOAT,		true, nullptr, nullptr, "GBuffer_Depth");
		gbuffers_surfaces.push_back(new CTexture2D(local_space, width_, height_, VK_FORMAT_R8G8B8A8_UNORM,	true, nullptr, nullptr, "GBuffer_Color"));
		gbuffers_surfaces.push_back(new CTexture2D(local_space, width_, height_, VK_FORMAT_R32G32_UINT,		true, nullptr, nullptr, "GBuffer_PositionU"));
		gbuffers_surfaces.push_back(new CTexture2D(local_space, width_, height_, VK_FORMAT_R32G32_UINT,		true, nullptr, nullptr, "GBuffer_NormalV"));
		gbuffers_surfaces.push_back(new CTexture2D(local_space, width_, height_, VK_FORMAT_R8G8B8A8_UNORM,	true, nullptr, nullptr, "GBuffer_MRST"));
		
		depth_surface = new CTexture2D(local_space, width_, height_, VK_FORMAT_D32_SFLOAT, true, nullptr, nullptr, "DepthSurface");

		postprocs_surfaces[0] = new CTexture2D(local_space, width_, height_, VK_FORMAT_B8G8R8A8_UNORM, true, nullptr, nullptr, "PostProcessed_0");
		postprocs_surfaces[1] = new CTexture2D(local_space, width_, height_, VK_FORMAT_B8G8R8A8_UNORM, true, nullptr, nullptr, "PostProcessed_1");

		FrameBuffers new_frame_buffers;
		new_frame_buffers.gbuffer			 = new CFrameBuffer(graphics->GetVkDevice(), gbuffers_surfaces, gbuffers_depth_surface, RenderPass::deferred);
		new_frame_buffers.canvas			 = new CFrameBuffer(graphics->GetVkDevice(), { postprocs_surfaces[1] }, depth_surface, RenderPass::forward);
		new_frame_buffers.post_processing[0] = new CFrameBuffer(graphics->GetVkDevice(), { postprocs_surfaces[0] }, nullptr, RenderPass::postproc);
		new_frame_buffers.post_processing[1] = new CFrameBuffer(graphics->GetVkDevice(), { postprocs_surfaces[1] }, nullptr, RenderPass::postproc);
		
		/*
		new_frame_buffers.layers.resize(layers_cameras.size());
		for (Uint i = 1 ; i < layers_cameras.size() ; i++)
		{
			layers_surfaces.at(i) = new CTexture2D(local_space, width_, height_, VK_FORMAT_B8G8R8A8_UNORM, true, nullptr, nullptr, "LayerSurface_" + std::to_string(i));
			new_frame_buffers.layers.at(i) = new CFrameBuffer(graphics->GetVkDevice(), { layers_surfaces.at(i) }, nullptr, RenderPass::postproc);
		}
		*/

		avalible_extents.insert(std::pair<Extent, FrameBuffers>(extent, new_frame_buffers));

		frame_buffers = new_frame_buffers;
	}
	else
	{
		extent = it->first;
	}

	//Bind the new gbuffer textures on the per-frame Descriptor Set(Worrning! -> this action is not Queue safe generaly! if i will work with many render ports I will should to create a queue of descriptors updates...

	for (Uint i = 0; i < graphics->m_FramesCount; i++)
	{
		VkDescriptorSet frame_descriptor_set = frames_info.at(i)->per_frame_descriptor_set;

		frame_buffers.gbuffer->BindSurfaces(frame_descriptor_set, 2, Engine::UsefulSamplers::regular_nearest);

		frame_buffers.canvas->BindSurfaces(frame_descriptor_set, 9, Engine::UsefulSamplers::regular_nearest);

		frame_buffers.post_processing[0]->BindSurfaces(frame_descriptor_set, 10, Engine::UsefulSamplers::regular_nearest);
		frame_buffers.post_processing[1]->BindSurfaces(frame_descriptor_set, 11, Engine::UsefulSamplers::regular_nearest);

		//for(Uint j = 1 ; j < frame_buffers.layers.size() ; j ++ )
			//frame_buffers.layers.at(j)->BindSurfaces(frames_info.at(i)->per_frame_descriptor_set, 12 + (j - 1), Engine::UsefulSamplers::regular_nearest);
	}
}

Engine::Renderer::Renderer()
{
	shades = new CAllocatedList<Shading*>(ENGINE_MAXIMUM_MATERIALS_COUNT);

	CreateLayout();
	CreateBasicGraphicsPipelines();

	frames_info.resize(3);
	frames_info.at(0) = new PerFrameInfo(this, 0);
	frames_info.at(1) = new PerFrameInfo(this, 1);
	frames_info.at(2) = new PerFrameInfo(this, 2);
}

Engine::Renderer::~Renderer()
{
	vkDestroyDescriptorSetLayout(graphics->GetVkDevice(), descriptor_set_layouts[DescriptorSetLayouts::PerResourceManager], nullptr);
	vkDestroyDescriptorSetLayout(graphics->GetVkDevice(), descriptor_set_layouts[DescriptorSetLayouts::PerGameObject], nullptr);
	vkDestroyDescriptorSetLayout(graphics->GetVkDevice(), descriptor_set_layouts[DescriptorSetLayouts::PerCamera], nullptr);
	vkDestroyDescriptorSetLayout(graphics->GetVkDevice(), descriptor_set_layouts[DescriptorSetLayouts::PerLight], nullptr);

	vkDestroyPipelineLayout(graphics->GetVkDevice(), layout, nullptr);
}