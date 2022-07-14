#include "CPlanetSurface.h"
#include "../../Engine/Engine_BasicMeshs.h"

CPlanetSurface::CPlanetSurface() : CObject()
{
	InputAttributes vertex_inputs = {	{0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Position
										{0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },		//Texcoord
										{0, 2, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Normals
										{0, 3, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Tangent
										{0, 4, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };	//Bitangent
	

	StandartPipelineInfo pipeline_info = {};
	pipeline_info.PipelineLayout			 = Engine::renderer->GetLayout();
	pipeline_info.PrimitiveTopology			 = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	pipeline_info.VertexStageInput			 = vertex_inputs;
	pipeline_info.VertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Surface.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.TesselationStage			 = TesselationStage(4, 
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Surface.tesc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Surface.tese", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
	pipeline_info.FargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Surface.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.RasterizationState		 = &VulkanUsefuls_RasterizerState(VULKAN_USEFULS_RASTERIZER_STATE_FILL);
	pipeline_info.MultisampleState			 = &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
	pipeline_info.ColorBlendState			 = &VulkanUsefuls_BlendState(VULKAN_USEFULS_BLEND_STATE_WRITEOVER, Engine::renderer->GetGBufferAttachmentsCount());
	pipeline_info.DepthStencilState			 = &VulkanUsefuls_DepthStancilState(VULKAN_USEFULS_DEPTH_STANCIL_STATE_LESS);
	pipeline_info.ExistingRenderPass		 = Engine::renderer->GetRenderPass().deferred;

	Engine::renderer->CreateShade("PlanetSurface", pipeline_info, Engine::Shade::Type::Deffered);
}

Void CPlanetSurface::GeneratePlanetMap(CGpuUploadTask* upload_task_)
{
	high_map = new CTextureCube(Engine::graphics->m_DeviceMemory.local_space, 512, 512, VK_FORMAT_R32_SFLOAT, true);
	Engine::resource_manager->AddTexturesPacket({ high_map });

	InputAttributes vertex_inputs = {	{0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Position
										{0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },		//Texcoord
										{0, 2, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Normals
										{0, 3, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Tangent
										{0, 4, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };	//Bitangent

	CRenderPass* render_pass = new CRenderPass(Engine::graphics->GetVkDevice(), { VK_FORMAT_R32_SFLOAT }, VK_FORMAT_UNDEFINED, RenderPassType::TextureCube);

	StandartPipelineInfo pipeline_info = {};
	pipeline_info.PipelineLayout			 = Engine::renderer->GetLayout();
	pipeline_info.PrimitiveTopology			 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipeline_info.VertexStageInput			 = vertex_inputs;
	pipeline_info.VertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/PlanetGeneration/SurfaceGeneration.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.GeometricStage			 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/PlanetGeneration/SurfaceGeneration.geom", VK_SHADER_STAGE_GEOMETRY_BIT);
	pipeline_info.FargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/PlanetGeneration/SurfaceGeneration.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.RasterizationState		 = &VulkanUsefuls_RasterizerState(VULKAN_USEFULS_RASTERIZER_STATE_FILL, VK_CULL_MODE_NONE);
	pipeline_info.MultisampleState			 = &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
	pipeline_info.ColorBlendState			 = &VulkanUsefuls_BlendState(VULKAN_USEFULS_BLEND_STATE_WRITEOVER, 1);
	pipeline_info.ExistingRenderPass		 = render_pass;

	CPipeline* generation_pipline = new CPipeline(Engine::graphics->GetVkDevice(), "TerrienGenerator", pipeline_info, false);
	CFrameBuffer* frame_buffer = new CFrameBuffer(Engine::graphics->GetVkDevice(), { high_map }, nullptr, render_pass);
			
	//Record dispach...
	VkDeviceSize stride = 0;
	Engine::SetViewport(upload_task_->GetCommandBuffer(), 0, 0, high_map->GetWidth(), high_map->GetHeight());
	generation_pipline->Bind(upload_task_->GetCommandBuffer());
	frame_buffer->Open(upload_task_->GetCommandBuffer());
	Engine::Meshs::Quad2D->Render(upload_task_);
	frame_buffer->Close(upload_task_->GetCommandBuffer());
}

Void CPlanetSurface::Init(CGpuUploadTask* upload_task_)
{
	//Generat planet map...
	GeneratePlanetMap(upload_task_);

	//All surfaces materials...

	CTexture2D* rock_color_texture		= new CTexture2D(Engine::graphics->m_DeviceMemory.local_space, L"Scene/Planet/Textures/rock0_color.jpg", upload_task_);
	CTexture2D* rock_normals_texture	= new CTexture2D(Engine::graphics->m_DeviceMemory.local_space, L"Scene/Planet/Textures/rock0_normal.png", upload_task_);
	CTexture2D* ground_color_texture	= new CTexture2D(Engine::graphics->m_DeviceMemory.local_space, L"Scene/Planet/Textures/ground0_color.jpg", upload_task_);

	Engine::resource_manager->AddTexturesPacket({ rock_color_texture, rock_normals_texture , ground_color_texture });
	
	CMaterial* material = new CMaterial("PlanetSurface");
	auto& material_data = material->Data<Material>();
	material_data.high_map_texloc	  = Engine::resource_manager->GetTextureLocation(high_map);
	material_data.rock_color_texloc   = Engine::resource_manager->GetTextureLocation(rock_color_texture);
	material_data.rock_normal_texloc  = Engine::resource_manager->GetTextureLocation(rock_normals_texture);
	material_data.ground_color_texloc = Engine::resource_manager->GetTextureLocation(ground_color_texture);
	
	Engine::resource_manager->UploadMaterialsPacket({ material }, upload_task_);
			
	//Control mesh...
	meshs.push_back(Engine::Meshs::NewControledCube(upload_task_, 32));
	meshs.at(0)->SetMaterial(material);

	//Set transform...
	radious = 100.0f;
	offset_transform[0][0] = radious;
}

Void CPlanetSurface::Render(CGpuDrawTask* draw_task_, Uint layer_)
{
}