#pragma once
#include "../../Engine/Engine_Renderer.h"

class CSun : public CLight
{
	private:
		CPipeline* pipeline;
		Vector4D color;

		struct ShaderInput
		{
			Vector3D	sun_center;
			Float		sun_radius;
			Vector4D	sun_color;
		};

	public:
		CSun(Vector4D color_) : CLight(), color(color_)
		{
			InputAttributes vertex_inputs = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX },		//Position
											  {0, 1, VK_FORMAT_R32G32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };		//Texcoords


			StandartPipelineInfo pipeline_info = {};
			pipeline_info.PipelineLayout			 = Engine::renderer->GetLayout();
			pipeline_info.PrimitiveTopology			 = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			pipeline_info.VertexStageInput			 = vertex_inputs;
			pipeline_info.VertexStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Sun.vert", VK_SHADER_STAGE_VERTEX_BIT);
			pipeline_info.FargmentStage				 = Engine::CompileGLSLShader(Engine::graphics->GetVkDevice(), L"Engine/Shaders/Planet/Sun.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
			pipeline_info.RasterizationState		 = &VulkanUsefuls_RasterizerState(VULKAN_USEFULS_RASTERIZER_STATE_FILL);
			pipeline_info.MultisampleState			 = &VulkanUsefuls_MultisamplerState(VULKAN_USEFULS_MULTISAMPLE_STATE_DEFAULT);
			pipeline_info.ColorBlendState			 = &VulkanUsefuls_BlendState(VULKAN_USEFULS_BLEND_STATE_REGULAR);
			pipeline_info.DepthStencilState			 = &VulkanUsefuls_DepthStancilState(VULKAN_USEFULS_DEPTH_STANCIL_STATE_LESS);
			pipeline_info.ExistingRenderPass		 = Engine::renderer->GetRenderPass().forward;

			pipeline = new CPipeline(Engine::graphics->GetVkDevice(), "Sun", pipeline_info, false);

			visible_mask = false;
		}

		Void Init(CGpuUploadTask* upload_task_) override
		{
			//Set transform...
			offset_transform = Scale(Vector3D(10, 10, 10));
		}

		Void Render(CGpuDrawTask* draw_task_) override
		{
			Vector3D	position;
			Vector3D	scale;
			Quaternion	orientation;
			DismantleTransform(GetTransform(), &position, &scale, &orientation);

			Matrix4D transform = GetTransform();

			Engine::GpuPropertiesBlockStruct shader_input_data;
			ShaderInput& shader_input = reinterpret_cast<ShaderInput&>(shader_input_data);
			shader_input.sun_center	  = position;
			shader_input.sun_radius	  = scale[0];
			shader_input.sun_color	  = color;
			Engine::renderer->AddPostProcessingCall(pipeline, shader_input_data, draw_task_->GetFrameIndex());
		}

}