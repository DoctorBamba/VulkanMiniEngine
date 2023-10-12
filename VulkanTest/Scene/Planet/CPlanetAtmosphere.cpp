#include "CPlanetAtmosphere.h"

CPlanetAtmosphere::CPlanetAtmosphere(Float radious_) : CObject("New_Planet_Atmosphere"), radious(radious_)
{
	InputAttributes vertexAttributes = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } }; //Sphere vertex position only
	
	PipelineState pipeline_info = {};
	pipeline_info.input.inputLayout			 = Engine::renderer->GetLayout();
	pipeline_info.input.topology			 = PrimitiveTopology::Triangles;
	pipeline_info.input.vertexAttributes	 = vertexAttributes;
	pipeline_info.vertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Framequad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.fargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Atmosphere.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.rasterization.faceCulling	 = FaceCulling::Front;
	pipeline_info.rasterization.polygonMode  = PolygonMode::Fill;
	pipeline_info.output.blendingStates		 = BlendingStatesDeclarationParser::Parse("Blend0 Add One Zero");//Override
	pipeline_info.output.depthState			 = AttachmentDepthState(true, true, ComparisonOperator::Less);
	pipeline_info.output.existingRenderPass	 = Engine::Renderer::RenderPass::output;

	pipeline = new CPipeline(Engine::graphics->GetVkDevice(), "PlanetAtmosphere", pipeline_info, false);
}

Void CPlanetAtmosphere::Init(CGpuUploadTask* upload_task_)
{

}

Void CPlanetAtmosphere::Draw(CGpuDrawTask* draw_task_, CCamera* camera)
{

	Engine::GpuPropertiesBlockStruct shader_input_data;
	ShaderInput& shader_input = reinterpret_cast<ShaderInput&>(shader_input_data);

	shader_input.planet_center				= Vector3D(0.0, 0.0, 0.0);
	shader_input.planet_radius				= 100.0;
	shader_input.atmosphere_radius			= 116.0;
	shader_input.atmosphere_rayleigh_coeff	= Vector3D(100.0e-5, 200.0e-5, 650.0e-5);
	shader_input.atmosphere_mie_coeff		= Vector3D(2.0e-3, 2.0e-3, 2.0e-3) * 2.0;
	shader_input.atmosphere_mie_scatt_coeff = 2.0e-3 * 1.2f;
	shader_input.sun_direction				= Vector3D(1.0, 1.0, 1.0) * (1.0f / Vector3D(1.0, 1.0, 1.0).lenght());
	shader_input.sun_intensity				= 40.0;
	shader_input.sun_color					= Vector4D(1.0, 0.82, 0.74, 1.0);

	camera->AddPostProcessingInvocation(pipeline, shader_input_data);
}