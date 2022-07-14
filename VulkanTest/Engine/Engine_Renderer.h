#pragma once

#include "../Scene/CScene.h"
#include "Engine_Graphics.h"
#include "SimpleComplie.h"
#include "Deafults/VulkanDefaultStates.h"


namespace Engine
{
	//The next class describe Shade Object.
	//Each visible GameObject's Material have a identefier of shade object that associate with.
	class Shade
	{
		public:
			enum class Type
			{
				Forward,
				Deffered
			};

		public:
			std::string name;
			Type		type;
			CPipeline*  pipeline;

			Uint	    instances_count;
			CObject**   instances;

			Shade(Type type_, CPipeline* pipeline_, std::string name_ = "New_Shade") : name(name_), type(type_), pipeline(pipeline_)
			{
				instances_count = 0;
				instances = new CObject*[ENGINE_MAXIMUM_GAMEOBJECTS_COUNT];
			}
	};


	//The next class describe the Engine's Renderer Object...
	class Renderer
	{
		struct PerFrameInfo
		{
			//Descriptors of the frame's resources...
			CDynamicUniformBuffer<GpuLookStruct, ENGINE_MAXIMUM_VIEWS_COUNT>*				look_ubuffer;
			CDynamicUniformBuffer<GpuObjectStruct, ENGINE_MAXIMUM_GAMEOBJECTS_COUNT>*		objects_ubuffer;
			CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>*					bones_offsets_ubuffer;
			CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>*					bones_poses_ubuffer;
			CDynamicUniformBuffer<GpuLightStruct, ENGINE_MAXIMUM_LIGHTS_COUNT>*				lights_gbuffer;
			CDynamicUniformBuffer<GpuPropertiesBlockStruct, ENGINE_MAXIMUM_POSTCALLS_COUNT>*	postcall_ubuffer;

			VkDescriptorSet per_frame_descriptor_set;
			VkDescriptorSet per_object_descriptor_set;
			VkDescriptorSet per_light_descriptor_set;

			PerFrameInfo(Renderer* renderer_context_, Uint frame_index_)
			{
				//Alocate descriptor sets...
				VkDescriptorSetAllocateInfo allocation_info{};
				allocation_info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocation_info.descriptorPool		= graphics->m_DescriptorPools.at(frame_index_);
				allocation_info.descriptorSetCount	= 1;
				allocation_info.pSetLayouts			= &renderer_context_->descriptor_sets_layout.per_frame;

				if (vkAllocateDescriptorSets(graphics->p_Device->device, &allocation_info, &per_frame_descriptor_set) != VK_SUCCESS)
				{
					throw std::runtime_error("PerFrameInfo::Constractor Error -> Failed to allocate descriptor sets!");
					return;
				}

				allocation_info.pSetLayouts			= &renderer_context_->descriptor_sets_layout.per_object;

				if (vkAllocateDescriptorSets(graphics->p_Device->device, &allocation_info, &per_object_descriptor_set) != VK_SUCCESS)
				{
					throw std::runtime_error("PerFrameInfo::Constractor Error -> Failed to allocate descriptor sets!");
					return;
				}

				allocation_info.pSetLayouts = &renderer_context_->descriptor_sets_layout.per_light;

				if (vkAllocateDescriptorSets(graphics->p_Device->device, &allocation_info, &per_light_descriptor_set) != VK_SUCCESS)
				{
					throw std::runtime_error("PerFrameInfo::Constractor Error -> Failed to allocate descriptor sets!");
					return;
				}

				look_ubuffer			= new CDynamicUniformBuffer<GpuLookStruct, ENGINE_MAXIMUM_VIEWS_COUNT>(graphics->m_DeviceMemory.uniform_space);
				postcall_ubuffer		= new CDynamicUniformBuffer<GpuPropertiesBlockStruct, ENGINE_MAXIMUM_POSTCALLS_COUNT>(graphics->m_DeviceMemory.uniform_space);
				objects_ubuffer			= new CDynamicUniformBuffer<GpuObjectStruct, ENGINE_MAXIMUM_GAMEOBJECTS_COUNT>(graphics->m_DeviceMemory.uniform_space);
				bones_offsets_ubuffer	= new CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>(graphics->m_DeviceMemory.uniform_space);
				bones_poses_ubuffer		= new CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>(graphics->m_DeviceMemory.uniform_space);
				lights_gbuffer			= new CDynamicUniformBuffer<GpuLightStruct, ENGINE_MAXIMUM_LIGHTS_COUNT>(graphics->m_DeviceMemory.uniform_space);
				

				look_ubuffer->Bind(per_frame_descriptor_set, 0);
				postcall_ubuffer->Bind(per_frame_descriptor_set, 1);
				objects_ubuffer->Bind(per_object_descriptor_set, 0);
				bones_offsets_ubuffer->Bind(per_object_descriptor_set, 1, 64);
				bones_poses_ubuffer->Bind(per_object_descriptor_set, 2, 64);
				lights_gbuffer->Bind(per_light_descriptor_set, 0);
			}
		};

		struct Extent
		{
			Uint16 width;
			Uint16 height;

			Extent() : Extent(0, 0) {}
			Extent(Uint16 width_, Uint16 height_) : width(width_), height(height_) {}
			Bool operator<(const Extent& other) const { return (Bool)(*((Uint*)&width) < *((Uint*)&other.width)); }
		};

		struct DescriptorSets
		{
			VkDescriptorSetLayout per_resources_manager;
			VkDescriptorSetLayout per_object;
			VkDescriptorSetLayout per_frame;
			VkDescriptorSetLayout per_light;
		}descriptor_sets_layout;

		struct RenderPass
		{
			CRenderPass* forward;
			CRenderPass* deferred;
			CRenderPass* postproc;
			CRenderPass* output;
		}render_pass;

		struct FrameBuffers
		{
			CFrameBuffer* gbuffer;
			CFrameBuffer* canvas;
			CFrameBuffer* post_processing[2];
			std::vector<CFrameBuffer*> layers;
		}frame_buffers;

		public:
			enum class DrawLayers
			{
				Base		= 0,
				Shadows		= 1,
				Reflections = 2
			};

		private:
			Uint gbuffer_attachments_count;

			std::map<Extent, FrameBuffers>	avalible_extents;
			Extent							extent;

			//Temporal varibles...
			std::vector<CCamera*>				layers_cameras;
			std::vector<CPipeline*>				post_processings_calls;

		public:
			VkPipelineLayout					layout;			//The Signature of the shaders
			std::vector<PerFrameInfo*>			frames_info;	//Per frame info
			CAllocatedList<Shade*>*				shades;			//List of all used shades
			std::map<std::string, Uint>			shades_ids;		//A map between shade name and it's id

		public:
			Renderer();
			~Renderer();

			Void CreateLayout();
			Void CreateBasicGraphicsPipelines();

			Void CreateShade(std::string name_, StandartPipelineInfo pipeline_desc_, Shade::Type shade_type_, Bool present_ = false);

			Void Resize(Uint16 width_, Uint16 height_);

			//Update buffers...
			Void UpdateLookBuffer(CCamera* camera_, Uint layout_, Uint frame_index_);
			Void UpdateObjectsBuffers(CObject* object_, Uint frame_index_);
			Void UpdateLightsBuffer(std::vector<CLight*> lights_, Uint frame_index_);
			Void UpdateBonesOffsetBuffer(CArmature* armature_, Uint frame_index_);
			Void UpdateBonesBuffer(CArmature* armature_, Uint frame_index_);
			Void UpdateSceneBuffers(CScene* object_, Uint frame_index_);

			//Binding...
			Void BindPipeline(VkCommandBuffer command_buffer_, std::string pipeline_name_);
			Void BindObject(CGpuDrawTask* draw_task_, CObject* object_);

			//Sorting...
			Void SortObject(CObject* object_, Uint layer_ = 0);
			Void SortScene(CScene* scene_, Uint layer_ = 0);

			//Draw...
			Void DrawObject(CGpuDrawTask* draw_task_, CObject* object_, Uint layer_ = 0);
			Void DrawScene(CGpuDrawTask* draw_task_, CScene* scene_, CFrameBuffer* output_,  Uint layer_ = 0);
			Void Render(CGpuDrawTask* draw_task_, CFrameBuffer* output_, CScene* scene_);



			VkPipelineLayout	GetLayout() { return layout; }
			RenderPass&			GetRenderPass() { return render_pass; }
			Uint				GetGBufferAttachmentsCount() { return gbuffer_attachments_count; }
			CCamera*			GetUsedCamere(Uint layer_) { return layers_cameras.at(layer_); }
			Uint				GetShadeId(std::string name_) 
			{ 
				auto it = shades_ids.find(name_); 
				if (it == shades_ids.end())
				{
					throw std::runtime_error("Renderer :: GetShadeId Error -> not shade with the given name is found!");
					return 0xffffffff;
				}

				return it->second;
			}

			//Add Effects...
			Uint AddNewLayer();
			Void AddPostProcessingCall(CPipeline* pipeline_, const GpuPropertiesBlockStruct& shader_input_, Uint frame_index_);

			
	};

	extern Renderer* renderer;
};