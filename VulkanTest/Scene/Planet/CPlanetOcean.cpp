#include "CPlanetOcean.h"
#include "../../Engine/Engine_BasicMeshs.h"

CPlanetOcean::CPlanetOcean() : CObject("PlanetOcean")
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
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.tesc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.tese", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
	pipeline_info.FargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.RasterizationState		 = &VulkanUsefuls_RasterizerState(VULKAN_USEFULS_RASTERIZER_STATE_FILL);
	pipeline_info.MultisampleState			 = &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
	pipeline_info.ColorBlendState			 = &VulkanUsefuls_BlendState(VULKAN_USEFULS_BLEND_STATE_REGULAR);
	pipeline_info.DepthStencilState			 = &VulkanUsefuls_DepthStancilState(VULKAN_USEFULS_DEPTH_STANCIL_STATE_LESS);
	pipeline_info.ExistingRenderPass		 = Engine::renderer->GetRenderPass().forward;

	Engine::renderer->CreateShade("PlanetOcean", pipeline_info, Engine::Shade::Type::Forward);

	visible_mask = 1;//Visible only on the main layer(not appear in reflections or shadows)
}

Void CPlanetOcean::Init(CGpuUploadTask* upload_task_)
{
	//Control mesh...

	CMaterial* material = new CMaterial("PlanetOcean");

	meshs.push_back(Engine::Meshs::NewControledCube(upload_task_, 32));
	meshs.at(0)->SetMaterial(material);

	//Set transform...
	radious = 100.0f - 0.05;
	offset_transform[0][0] = radious;

	reflection_layer_id = Engine::renderer->AddNewLayer();
}

Void CPlanetOcean::Render(CGpuDrawTask* draw_task_, Uint layer_)
{
	//Build a reflec camera...
	CPersCamera* orginal_camera = (dynamic_cast<CPersCamera*>(Engine::renderer->GetUsedCamere(0)));
	reflection_layer_camera = new CPersCamera(*orginal_camera);
			
	const Vector3D camera_position = orginal_camera->GetPosition();

	const Vector3D n = Normalize(camera_position - GetPosition());
	const Vector3D p = radious * n;

	reflection_layer_camera->SetOffsetTransform(Translate(p + Reflect(camera_position - p, n)));
	reflection_layer_camera->SetLookPoint(p + Reflect(orginal_camera->GetLookPoint() - p, n));
	reflection_layer_camera->SetUp(Reflect(reflection_layer_camera->GetUp(), n));
	reflection_layer_camera->SetClipPlane(Vector4D(n[0], n[1], n[2], radious));

	Engine::renderer->UpdateLookBuffer(reflection_layer_camera, reflection_layer_id, draw_task_->GetFrameIndex());
}
