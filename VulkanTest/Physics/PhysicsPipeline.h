#pragma once
#include "Collider.h"

#include "../Graphics/ComputePipeline.h"
#include "../Graphics/Buffers/CStorgeBuffer.h"
#include "../Graphics/CAllocator.h"
#include "../Engine/SimpleComplie.h"

namespace Physics
{
	typedef Uint RigidBodyIdentefier;


	struct GpuTypes
	{
		typedef Uint Padding;

		struct RigidBody
		{
			Uint transform_identefier;
			Uint colliderType;
			Uint dynamic;
			Uint A;

			Vector4D inertia;					//Angular & Linear(mass) inertia
			Vector4D position;
			Matrix3D orientation;
			Vector3D extents;
			Vector4D linear_velocity;
			Vector4D angular_velocity;
		};

		struct Collision
		{
			Vector4D intersection_points[48];
			Vector4D best_saperator;			//The best saperation plane between the tow colliders
		};
	};


	class CPhysicsPipeline
	{
		public:

			static const Uint MaximumRigidBodeisCount = 65536;
			static const Uint MaximumRigidBodyPerChunk = 1024;
			static const Uint MaximumDescriptorsSets = 1024;

		private:


			enum DescriptorSetLayouts
			{
				PerTick,
				COUNT
			};

			VkDescriptorSetLayout descriptor_set_layouts[DescriptorSetLayouts::COUNT];
			VkDescriptorSet p_PerTickDescriptorSet;

			//The Gpu-Queue for the physics operations execution and a proper commands pool
			VkQueue			p_ComputeQueue			= VK_NULL_HANDLE;
			VkCommandPool	p_ComputeCommandsPool	= VK_NULL_HANDLE;
			Void CreateComputeQueueAndCommandsPool();

			//The Input Layout of the Compute Shaders in this physics pipeline
			VkPipelineLayout p_InputLayout;
			Void CreateInputLayout();

			//Cpu to Gpu uploading...
			CGpuUploadTask* p_TickTask;
			CIntermidiateBuffer* p_TransformFeedbackIntermidiate;

			//Buffers...
			CBasicAllocator*					p_RigidBodeisAllocator;
			CStorgeBuffer<GpuTypes::RigidBody>* p_RigidBodeisBuffer;

			CStorgeBuffer<GpuTypes::Collision>* p_CollisionsBuffer;
			CStorgeBuffer<Uint>*				p_CollisionsCountBuffer;

			//Compute Shaders...
			CComputePipeline* p_CollisionDetectionPipeline;


			CVulkanDevice* const p_Device;
		public:

			CPhysicsPipeline(CVulkanDevice* device_) : p_Device(device_)
			{
				CreateComputeQueueAndCommandsPool();
				CreateInputLayout();


				p_TickTask = new CGpuUploadTask(device_, p_ComputeCommandsPool);

				p_RigidBodeisAllocator = new CBasicAllocator(MaximumRigidBodeisCount);
				p_RigidBodeisBuffer = new CStorgeBuffer<GpuTypes::RigidBody>(device_, MaximumRigidBodyPerChunk);


				p_CollisionsBuffer		= new CStorgeBuffer<GpuTypes::Collision>(device_, MaximumRigidBodyPerChunk * MaximumRigidBodyPerChunk);
				p_CollisionsCountBuffer = new CStorgeBuffer<Uint>(device_, 1);


				p_CollisionDetectionPipeline = new CComputePipeline(device_, "CollisionDetection", Engine::CompileGLSLShader(device_->device, L"Physics/ComputeShaders/CollisionDetection.compute", VK_SHADER_STAGE_COMPUTE_BIT), p_InputLayout);
			
				p_TransformFeedbackIntermidiate = new CIntermidiateBuffer(device_, sizeof(GpuTypes::RigidBody) * MaximumRigidBodyPerChunk, CIntermidiateBuffer::Usage::Download);
			
				//Create and write descriptor set...

				p_PerTickDescriptorSet = p_Device->AllocateDescriptorSet(descriptor_set_layouts[DescriptorSetLayouts::PerTick]);
				p_RigidBodeisBuffer->Bind(p_PerTickDescriptorSet, 1);
				p_CollisionsBuffer->Bind(p_PerTickDescriptorSet, 2);
				p_CollisionsCountBuffer->Bind(p_PerTickDescriptorSet, 3);
			}

		

			Void Tick()
			{
				vkCmdBindDescriptorSets(p_TickTask->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, p_InputLayout,
										0, 1, &p_PerTickDescriptorSet, 0, nullptr);

				p_CollisionDetectionPipeline->Dispacth(p_TickTask, (Uint)ceil((float)MaximumRigidBodyPerChunk / 8.0f),
																   (Uint)ceil((float)MaximumRigidBodyPerChunk / 8.0f),
																   1);


				p_RigidBodeisBuffer->Download(p_TickTask, p_TransformFeedbackIntermidiate);

				p_TickTask->Execute(p_ComputeQueue);
				p_TickTask->WaitAntilComplite();
				p_TickTask->Reset();

				UpdateTransforms();
			}

			RigidBodyIdentefier NewRigidBody(Collider collider_, Matrix4D* const transform_, float mass_, Bool dynamic_, const Vector3D& initial_velocity_ = Vector3D::Zero, const Vector3D& initial_angular_velocity_ = Vector3D::Zero)
			{
				RigidBodyIdentefier identefier = p_RigidBodeisAllocator->AllocateElement();

				Vector3D initial_position;
				Matrix3D initial_orientation;
				Vector3D initial_scale;
				DecomposeTransform(*transform_, &initial_position, &initial_scale, &initial_orientation);

				CIntermidiateBuffer* intermidiate = new CIntermidiateBuffer(p_Device, sizeof(GpuTypes::RigidBody), CIntermidiateBuffer::Usage::Upload);
				
				GpuTypes::RigidBody* gputype_rigidbody = (GpuTypes::RigidBody*)intermidiate->Map();
				gputype_rigidbody->transform_identefier	= (Uint)transform_;
				gputype_rigidbody->colliderType			= (Uint)collider_.collider_type;
				gputype_rigidbody->dynamic				= dynamic_ ? 1 : 0;
				gputype_rigidbody->A					= 0;


				gputype_rigidbody->inertia			= mass_ * collider_.GetInertiaFactors();
				gputype_rigidbody->position			= Make4D(initial_position);
				gputype_rigidbody->extents			= collider_.extents;
				gputype_rigidbody->orientation		= initial_orientation;
				gputype_rigidbody->linear_velocity	= Make4D(initial_velocity_);
				gputype_rigidbody->angular_velocity	= Make4D(initial_angular_velocity_);

				intermidiate->Unmap();


				p_RigidBodeisBuffer->Upload(p_TickTask, intermidiate, identefier * sizeof(GpuTypes::RigidBody));
			}

			Void UpdateTransforms()
			{
				GpuTypes::RigidBody* rigidbodeisFeedbackData = (GpuTypes::RigidBody*)p_TransformFeedbackIntermidiate->Map();
				for (int i = 0; i < MaximumRigidBodyPerChunk; i++)
				{
					const GpuTypes::RigidBody& rigidbody = rigidbodeisFeedbackData[i];
					Matrix4D* transform = reinterpret_cast<Matrix4D*>(rigidbody.transform_identefier);
					*transform = Transform(Make3D(rigidbody.position), rigidbody.orientation, rigidbody.extents);
				}

				p_TransformFeedbackIntermidiate->Unmap();
			}

	};
}