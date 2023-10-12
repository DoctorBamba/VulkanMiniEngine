#pragma once

#include "../Scene/CScene.h"
#include "Engine_Graphics.h"
#include "SimpleComplie.h"
#include "Deafults/VulkanDefaultStates.h"


namespace Engine
{
	//The next class describe Shading Object.
	//Each visible GameObject's Material have a identefier of shade object that associate with.
	class Shading
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

			Shading(Type type_, CPipeline* pipeline_, std::string name_ = "NewShading") : name(name_), type(type_), pipeline(pipeline_)
			{
				instances_count = 0;
				instances = new CObject*[ENGINE_MAXIMUM_GAMEOBJECTS_COUNT];
			}
	};

	struct CameraBuffersLoc
	{
		Bool bounded;
		Uint depth_loc;
		Uint color_loc[MAXIMUM_ATTACHMENTS_PER_RENDERPASS];

		CameraBuffersLoc()
		{
			bounded = false;

			depth_loc = UNUSED;

			for (Uint i = 0; i < MAXIMUM_ATTACHMENTS_PER_RENDERPASS; i++)
				color_loc[i] = UNUSED;
		}
	};

	//The next class describe the Engine's Renderer Object...
	class Renderer
	{
		struct PerFrameInfo
		{
			//Descriptors of the frame's resources...
			CDynamicUniformBuffer<GpuLookStruct, ENGINE_MAXIMUM_CAMERAS_COUNT>*					look_ubuffer;
			CDynamicUniformBuffer<GpuObjectStruct, ENGINE_MAXIMUM_GAMEOBJECTS_COUNT>*			objects_ubuffer;
			CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>*						bones_offsets_ubuffer;
			CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>*						bones_poses_ubuffer;
			CDynamicUniformBuffer<GpuLightStruct, ENGINE_MAXIMUM_LIGHTS_COUNT>*					lights_gbuffer;
			CDynamicUniformBuffer<GpuPropertiesBlockStruct, ENGINE_MAXIMUM_PP_INVOCS_COUNT>*	pp_invocation_ubuffer;

			VkDescriptorSet per_frame_descriptor_set;
			VkDescriptorSet per_object_descriptor_set;
			VkDescriptorSet per_light_descriptor_set;


			PerFrameInfo(Renderer* renderer_, Uint frame_index_)
			{
				per_frame_descriptor_set	= graphics->p_Device->AllocateDescriptorSet(renderer_->descriptor_set_layouts[DescriptorSetLayouts::PerCamera]);
				per_object_descriptor_set	= graphics->p_Device->AllocateDescriptorSet(renderer_->descriptor_set_layouts[DescriptorSetLayouts::PerGameObject]);
				per_light_descriptor_set	= graphics->p_Device->AllocateDescriptorSet(renderer_->descriptor_set_layouts[DescriptorSetLayouts::PerLight]);

				look_ubuffer			= new CDynamicUniformBuffer<GpuLookStruct, ENGINE_MAXIMUM_CAMERAS_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				pp_invocation_ubuffer	= new CDynamicUniformBuffer<GpuPropertiesBlockStruct, ENGINE_MAXIMUM_PP_INVOCS_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				objects_ubuffer			= new CDynamicUniformBuffer<GpuObjectStruct, ENGINE_MAXIMUM_GAMEOBJECTS_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				bones_offsets_ubuffer	= new CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				bones_poses_ubuffer		= new CDynamicUniformBuffer<Matrix4D, ENGINE_MAXIMUM_BONES_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				lights_gbuffer			= new CDynamicUniformBuffer<GpuLightStruct, ENGINE_MAXIMUM_LIGHTS_COUNT>(graphics->p_Device->MemorySpaces.uniform_space);
				

				look_ubuffer->Bind(per_frame_descriptor_set, 0);
				pp_invocation_ubuffer->Bind(per_frame_descriptor_set, 1);
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

		struct DescriptorSetLayouts
		{
			enum
			{
				PerResourceManager	= 0,
				PerCamera			= 1,
				PerGameObject		= 2,
				PerLight			= 3,
				COUNT
			};
		};

		VkDescriptorSetLayout descriptor_set_layouts[DescriptorSetLayouts::COUNT];

		struct FrameBuffers
		{
			CFrameBuffer* gbuffer;
			CFrameBuffer* canvas;
			CFrameBuffer* post_processing[2];
		}frame_buffers;

		public:

			static const Uint GBufferAttachmentsCount = 4;

			struct RenderPass
			{
				inline static CRenderPass* forward;
				inline static CRenderPass* deferred;
				inline static CRenderPass* postproc;
				inline static CRenderPass* output;
			};

			static void InitialStatics(CVulkanDevice* device);


			enum class DrawLayers
			{
				Base		= 0,
				Shadows		= 1,
				Reflections = 2
			};
			
		private:
			CameraBuffersLoc bounded_cameras[ENGINE_MAXIMUM_CAMERAS_COUNT];
			std::map<Extent, FrameBuffers>	avalible_extents;
			Extent							extent;


		public:
			VkPipelineLayout					layout;			//The Signature of the shaders
			std::vector<PerFrameInfo*>			frames_info;	//Per frame info
			CAllocatedList<Shading*>*			shades;			//List of all used shades
			std::map<std::string, Uint>			shades_ids;		//A map between shade name and it's id

		public:
			Renderer();
			~Renderer();

			Void CreateLayout();
			Void CreateBasicGraphicsPipelines();
			Void CreateShade(std::string name_, PipelineState pipeline_desc_, Shading::Type shade_type_, Bool present_ = false);

			Void Resize(Uint16 width_, Uint16 height_);

			//Update buffers...
			Void UpdateLookBuffer(CCamera* camera_, Uint frame_index_);
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
			Void DrawScene(CGpuDrawTask* draw_task_, CScene* scene_, CCamera* Camera);
			Void Draw(CGpuDrawTask* draw_task_, CScene* scene_);


			VkPipelineLayout	GetLayout() { return layout; }
			Uint				GetGBufferAttachmentsCount() { return GBufferAttachmentsCount; }
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
	};

	extern Renderer* renderer;
};