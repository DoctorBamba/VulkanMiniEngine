#include "CPlanetAtmosphere.h"

CPlanetAtmosphere::CPlanetAtmosphere(Float radious_) : CObject("New_Planet_Atmosphere"), radious(radious_)
{
	InputAttributes vertex_inputs = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } }; //Sphere vertex position only

	StandartPipelineInfo pipeline_info = {};
	pipeline_info.PipelineLayout			 = Engine::renderer->GetLayout();
	pipeline_info.PrimitiveTopology			 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipeline_info.VertexStageInput			 = vertex_inputs;
	pipeline_info.VertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Framequad.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.FargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Atmosphere.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.RasterizationState		 = &VulkanUsefuls_RasterizerState(VULKAN_USEFULS_RASTERIZER_STATE_FILL);
	pipeline_info.MultisampleState			 = &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
	pipeline_info.ColorBlendState			 = &VulkanUsefuls_BlendState(VULKAN_USEFULS_BLEND_STATE_WRITEOVER, 1);
	pipeline_info.DepthStencilState			 = &VulkanUsefuls_DepthStancilState(VULKAN_USEFULS_DEPTH_STANCIL_STATE_LESS);
	pipeline_info.ExistingRenderPass		 = Engine::renderer->GetRenderPass().output;

	pipeline = new CPipeline(Engine::graphics->GetVkDevice(), "PlanetAtmosphere", pipeline_info, false);
}

Void CPlanetAtmosphere::Init(CGpuUploadTask* upload_task_)
{

}

Void CPlanetAtmosphere::Render(CGpuDrawTask* draw_task_, Uint layer_)
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

	Engine::renderer->AddPostProcessingCall(pipeline, shader_input_data, draw_task_->GetFrameIndex());
}