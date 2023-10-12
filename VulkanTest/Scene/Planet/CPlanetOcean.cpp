#include "CPlanetOcean.h"
#include "../../Engine/Engine_BasicMeshs.h"

CPlanetOcean::CPlanetOcean() : CObject("PlanetOcean")
{
	InputAttributes vertex_attributes = {	{0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Position
											{0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },		//Texcoord
											{0, 2, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Normals
											{0, 3, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },	//Tangent
											{0, 4, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };	//Bitangent

	PipelineState pipeline_info = {};
	pipeline_info.input.inputLayout			 = Engine::renderer->GetLayout();
	pipeline_info.input.topology			 = PrimitiveTopology::PatchList;
	pipeline_info.input.controlPointCount	 = 4;
	pipeline_info.input.vertexAttributes	 = vertex_attributes;
	pipeline_info.vertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Surface.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.tesselationStage			 = TesselationStage(4,
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.tesc", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
											   Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.tese", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT));
	pipeline_info.fargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Ocean.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.rasterization.faceCulling	 = FaceCulling::Front;
	pipeline_info.rasterization.polygonMode  = PolygonMode::Fill;
	pipeline_info.output.blendingStates		 = BlendingStatesDeclarationParser::Parse("Blend0 Add One One");
	pipeline_info.output.depthState			 = AttachmentDepthState(true, true, ComparisonOperator::Less);
	pipeline_info.output.existingRenderPass	 = Engine::Renderer::RenderPass::forward;

	Engine::renderer->CreateShade("PlanetOcean", pipeline_info, Engine::Shading::Type::Forward);

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
}

Void CPlanetOcean::Draw(CGpuDrawTask* draw_task_, CCamera* camera)
{
	//Build a reflec camera...
	
	
}
