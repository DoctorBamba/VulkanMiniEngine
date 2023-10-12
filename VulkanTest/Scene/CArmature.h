#pragma once

#include "CGameObject.h"
#include "CAnimation.h"
#include <set>

#include "../Graphics/Buffers/CUniformBuffer.h"

#define MAXIMUM_BONES_COUNT_PER_ARMATURE	1024
#define MAXIMUM_BONES_CONNECTIONS			4

typedef Float (*InterpFunction)(Float);

static Float LINEAR_INTERPOLATION_FUNC(Float t)
{
	return t;
}

//Armature object class...

class CArmature : public CObject
{
	friend Engine::ObjectsManager;
	friend Engine::Renderer;

	protected:
		std::map<std::string, std::vector<Uint>> bones_map;//Maping betwin bones name and bone's index in the bones-buffer
		
		std::vector<Matrix4D>					bones_poses;
		std::vector<Matrix4D>					bones_offset;

		CUniformBuffer<Matrix4D[MAXIMUM_BONES_COUNT_PER_ARMATURE]>** gpu_bones_poses;//This is a dynamic const-buffer so we need buffer per back-buffer
		CUniformBuffer<Matrix4D[MAXIMUM_BONES_COUNT_PER_ARMATURE]>*  gpu_bones_offsets;//This is a constant const-buffer

		Uint armature_offset;

	public:
		typedef std::vector<Matrix4D> State;

		CArmature() : CObject("New_Armature") {}
		CArmature(CObject* source_);
		
		Void GetInfluanceObjects(CObject* object_, std::set<CObject*>* influences_);
		Void GetInfluanceObjects(std::set<CObject*>* influences_);
		Void CreateBonesTransforms();
		
		Void ApplyAnimation(const CAnimation *animation_, Double time_, InterpFunction interpolation_func_);//Active one of the relevant animation(set the bones transform)
		//PE_Void ActiveBlendedAnimations(CAnimation* first_animation_, PE_Float first_time_, CAnimation* secound_animation_, PE_Float secound_time_, PE_Float blend_factor_, InterpFunction interpolation_func_);
		//PE_Void ActiveDeflectedAnimation(CAnimation* animation_, PE_Double time_, CArmature::State state_, PE_Float deflect_factor_, InterpFunction interpolation_func_);
		Void RecursiveUpdate(const CAnimation* animation_, Float animation_time_, InterpFunction interpolation_func_);
		Void RecursiveBonesUpdate(const CAnimation* animation_, Float animation_time_, CObject* object_, Matrix4D perant_transform_, InterpFunction interpolation_func_);
		
		Void CaptureState(CArmature::State* state_);
		Void UpdateBonesBuffers(Uint frame_index_);

		//D3D12_GPU_VIRTUAL_ADDRESS GetGpuBonesPoseBufferAddres(PE_Uint frame_index_) { return gpu_bones_poses[frame_index_]->GetGpuBufferAddress(); }
		//D3D12_GPU_VIRTUAL_ADDRESS GetGpuBonesOffsetsBufferAddres() { return gpu_bones_offsets->GetGpuBufferAddress(); }
};

//The next function make a free c++ object(class-instance of class the user can program) from CArmature...
template<class Obj> Obj* MakeArmatureObject(CArmature* armature);


//Bone object...
class CBone : public CObject
{
	friend CScene;
	friend CArmature;

	private:
		std::vector<CObject*> influences;

	public:
		CBone(CObject* source_) : CObject(source_) {}
};
