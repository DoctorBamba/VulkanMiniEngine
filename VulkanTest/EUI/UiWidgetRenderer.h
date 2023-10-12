#pragma once
#include "UiFonts.h"
#include "UiWidget.h"

#include "UiGraphics.h"

struct UiGpuWidgetInstance
{
	Vector3D			m_UtRow0;
	Uint				m_Width;
	Vector3D			m_UtRow1;
	Uint				m_Height;

	UiGpuWidgetInstance(UiCoords position_, Uint16 width_, Uint16 height_, Float angle_)
	{
		Float c = cos(angle_);
		Float s = sin(angle_);

		m_UtRow0 = Vector3D(c, -s, (Float)position_[0]);
		m_UtRow1 = Vector3D(s, c, (Float)position_[1]);

		m_Width = width_;
		m_Height = height_;
	}
};

struct UiDepthLayer
{
	std::list<UiWidgetId>		m_widgetsContent;
	std::list<UiFontInstance>	m_textsContent;

	Void Reset()
	{
		m_widgetsContent.clear();
		m_textsContent.clear();
	}
};


class UiWidgetRenderer
{
	public:
		struct DescriptorSetLayouts
		{
			enum
			{
				PerRenderer = 0,
				PerWidget	= 1,
				COUNT
			};
		};

		static VkDescriptorSetLayout DescriptorSetLayouts[DescriptorSetLayouts::COUNT];

		struct GraphicsFrameInfo
		{
			VkDescriptorSet m_PerRenderingDescreptorSet;
			VkDescriptorSet m_WidgetDescriptorSets[UI_MAXIMUM_NUMBER_OF_WIDGETS];
			CDynamicUniformBuffer<UiGpuWidgetInstance, UI_MAXIMUM_NUMBER_OF_WIDGETS>* m_WidgetUbuffer;


			GraphicsFrameInfo(Uint frame_index_)
			{
				//Alocate descriptor sets per evrey location in widgets pool...

				m_PerRenderingDescreptorSet = UI::Graphics->p_Device->AllocateDescriptorSet(DescriptorSetLayouts[DescriptorSetLayouts::PerRenderer]);
				for (Uint i = 0; i < UI_MAXIMUM_NUMBER_OF_WIDGETS; i++)
					m_WidgetDescriptorSets[i] = UI::Graphics->p_Device->AllocateDescriptorSet(DescriptorSetLayouts[DescriptorSetLayouts::PerWidget]);

				//Create the Widget buffer and bind it on all of the descriptor sets... 
				m_WidgetUbuffer = new CDynamicUniformBuffer<UiGpuWidgetInstance, UI_MAXIMUM_NUMBER_OF_WIDGETS>(UI::Graphics->p_Device->MemorySpaces.local_space);
				
				for (Uint i = 0; i < UI_MAXIMUM_NUMBER_OF_WIDGETS; i++)
					m_WidgetUbuffer->Bind(m_WidgetDescriptorSets[i], 0);
			}
		};

	protected:
		VkPipelineLayout p_layout;
		CFrameBuffer*	 p_output;

		std::map<Float, UiDepthLayer*>  m_depthLayers;
		std::vector<GraphicsFrameInfo>  m_framesInfo;

		CPipeline* p_deafultPipeline;

	public:

		UiWidgetRenderer();

		Void CreateLayout();
		CPipeline* CreateWidgetsPipeline(std::wstring fragament_path_);

		Void UpdateWidgets(UiWidget* widget_, Uint frame_index_);
		Void DrawWidget(CGpuDrawTask* draw_tast_, UiWidgetId id_);
		Void DrawAllLayers(CGpuDrawTask* draw_tast_);
};