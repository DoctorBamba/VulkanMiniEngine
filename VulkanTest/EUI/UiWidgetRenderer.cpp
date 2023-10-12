#include "UiWidgetRenderer.h"

using namespace UI;

VkDescriptorSetLayout UiWidgetRenderer::DescriptorSetLayouts[UiWidgetRenderer::DescriptorSetLayouts::COUNT];

UiWidgetRenderer::UiWidgetRenderer()
{
	CreateLayout();

	m_framesInfo.clear();
	for (Uint i = 0; i < Graphics->m_FramesCount; i++)
		m_framesInfo.push_back(GraphicsFrameInfo(i));

	p_deafultPipeline = CreateWidgetsPipeline(L"EUI/Shaders/Deafult.frag");

	int  y = 5;
}

CPipeline* UiWidgetRenderer::CreateWidgetsPipeline(std::wstring fragament_path_)
{
	InputAttributes vertex_attributes = { {0, 0, VK_FORMAT_R32G32B32_SFLOAT, VK_VERTEX_INPUT_RATE_VERTEX } };

	PipelineState pipeline_info = {};
	pipeline_info.input.inputLayout			 = p_layout;
	pipeline_info.input.topology			 = PrimitiveTopology::Triangles;
	pipeline_info.input.vertexAttributes	 = vertex_attributes;
	pipeline_info.vertexStage				 = Engine::CompileGLSLShader(Graphics->GetVkDevice(), L"EUI/Shaders/widget.vert", VK_SHADER_STAGE_VERTEX_BIT);
	pipeline_info.fargmentStage				 = Engine::CompileGLSLShader(Graphics->GetVkDevice(), fragament_path_, VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline_info.rasterization.faceCulling	 = FaceCulling::Front;
	pipeline_info.rasterization.polygonMode	 = PolygonMode::Fill;
	pipeline_info.output.blendingStates		 = BlendingStatesDeclarationParser::Parse("Blend0 Add One One");
	pipeline_info.output.depthState			 = AttachmentDepthState(true, true, ComparisonOperator::Less);
	pipeline_info.output.colorFormats		 = { VK_FORMAT_R8G8B8A8_UNORM };
	pipeline_info.output .depthFormat		 = VK_FORMAT_D24_UNORM_S8_UINT;

	return new CPipeline(Engine::graphics->GetVkDevice(), "Widgets Pipeline", pipeline_info, false);
}


Void UiWidgetRenderer::CreateLayout()
{
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_desc;

	//Per Renderer descriptor set...

	VkDescriptorSetLayoutBinding per_renderer_binds[1];

	VkDescriptorSetLayoutBinding& viewport_binding = per_renderer_binds[0];
	viewport_binding = {};
	viewport_binding.binding			= 0;
	viewport_binding.descriptorType		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	viewport_binding.descriptorCount	= 1;
	viewport_binding.stageFlags			= VK_SHADER_STAGE_ALL;

	
	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(per_renderer_binds);
	descriptor_set_layout_desc.pBindings	= per_renderer_binds;

	if (vkCreateDescriptorSetLayout(Graphics->GetVkDevice(), &descriptor_set_layout_desc, nullptr, &DescriptorSetLayouts[DescriptorSetLayouts::PerRenderer]) != VK_SUCCESS)
	{
		throw std::runtime_error("UiWidgetRenderer::CreateLayout -> Failed to create descriptor set layout!");
		return;
	}

	//Per Widget descriptor set...

	VkDescriptorSetLayoutBinding per_widget_binds[2];

	VkDescriptorSetLayoutBinding& widget_binding = per_widget_binds[0];
	widget_binding = {};
	widget_binding.binding			= 0;
	widget_binding.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	widget_binding.descriptorCount	= 1;
	widget_binding.stageFlags		= VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutBinding& texture_binding = per_widget_binds[1];
	texture_binding = {};
	texture_binding.binding			= 1;
	texture_binding.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	texture_binding.descriptorCount	= 1;
	texture_binding.stageFlags		= VK_SHADER_STAGE_ALL;


	descriptor_set_layout_desc = {};
	descriptor_set_layout_desc.sType		= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_desc.bindingCount = array_size(per_widget_binds);
	descriptor_set_layout_desc.pBindings	= per_widget_binds;

	if (vkCreateDescriptorSetLayout(Graphics->GetVkDevice(), &descriptor_set_layout_desc, nullptr, &DescriptorSetLayouts[DescriptorSetLayouts::PerWidget]) != VK_SUCCESS)
	{
		throw std::runtime_error("UiWidgetRenderer::CreateLayout -> Failed to create descriptor set layout!");
		return;
	}

	//Pipeline Layout...

	VkPipelineLayout pipe_line_layout;
	VkPipelineLayoutCreateInfo pipeline_layout_desc{};
	pipeline_layout_desc.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_desc.setLayoutCount				= DescriptorSetLayouts::COUNT;
	pipeline_layout_desc.pSetLayouts				= DescriptorSetLayouts;
	pipeline_layout_desc.pushConstantRangeCount		= 0;
	pipeline_layout_desc.pPushConstantRanges		= nullptr;

	if (vkCreatePipelineLayout(Graphics->GetVkDevice(), &pipeline_layout_desc, nullptr, &p_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("UiWidgetRenderer::CreateLayout -> Failed to create pipeline layout!");
		return;
	}
}

Void UiWidgetRenderer::UpdateWidgets(UiWidget* widget_, Uint frame_index_)
{
	//Update buffers and binding at the widget's id...
	UiWidgetId		widget_id = widget_->GetWidgetId();
	VkDescriptorSet descriptor_set = m_framesInfo.at(frame_index_).m_WidgetDescriptorSets[widget_id];

	UiGpuWidgetInstance gpu_widget = UiGpuWidgetInstance(widget_->GetPosition(), widget_->GetWidth(), widget_->GetHeight(), widget_->GetAngle());
	m_framesInfo.at(frame_index_).m_WidgetUbuffer->Update(gpu_widget, widget_id);

	if (widget_->m_MainTexture != nullptr && widget_->m_MainTexture->IsBoundable())
		widget_->m_MainTexture->Bind(m_framesInfo.at(frame_index_).m_WidgetDescriptorSets[widget_->GetWidgetId()], 1);

	//Add new widget to the corresponding depth layout...
	UiDepthLayer* depth_layer = m_depthLayers.emplace(widget_->GetDepth(), new UiDepthLayer()).first->second;
	depth_layer->m_widgetsContent.push_back(widget_->GetWidgetId());

	for (auto it = widget_->GetChildrens().begin(); it != widget_->GetChildrens().end(); it++)
		UpdateWidgets(*it, frame_index_);
}

Void UiWidgetRenderer::DrawWidget(CGpuDrawTask* draw_tast_, UiWidgetId id_)
{
	VkDescriptorSet descriptor_set = m_framesInfo.at(draw_tast_->GetFrameIndex()).m_WidgetDescriptorSets[id_];


	vkCmdBindDescriptorSets(draw_tast_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, p_layout,
							DescriptorSetLayouts::PerWidget, 1, &descriptor_set, 1, &id_);

	Engine::Meshs::Quad2D->Draw(draw_tast_);
}

Void UiWidgetRenderer::DrawAllLayers(CGpuDrawTask* draw_tast_)
{
	p_output->Open(draw_tast_->GetCommandBuffer());
	p_deafultPipeline->Bind(draw_tast_->GetCommandBuffer());

	for (auto dl_it = m_depthLayers.begin(); dl_it != m_depthLayers.end(); dl_it)
	{
		UiDepthLayer* depth_layer = dl_it->second;

		for (auto w_it = depth_layer->m_widgetsContent.begin(); w_it != depth_layer->m_widgetsContent.end(); w_it++)
			DrawWidget(draw_tast_, (*w_it));

		depth_layer->Reset();
	}

	p_output->Close(draw_tast_->GetCommandBuffer());
}