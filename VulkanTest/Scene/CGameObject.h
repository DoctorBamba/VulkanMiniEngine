#pragma once

#include "CMesh.h"
#include "../Engine/Engine_GameObjectsManager.h"//For Generate objects ids

namespace Engine
{
	class Renderer;
}

class CObject
{
	friend Engine::ObjectsManager;
	friend Engine::Renderer;

	friend class CScene;
	friend Void CObject::AddChild(CObject* new_child_);
	
	public:
		struct ArmatureLink//Meshs Modefier link
		{
			CObject* armature;
			std::vector<Uint> bones_internal_offset;

			Uint GetBonesOffset(Uint mesh_index_);
		}armature_link;

	protected:

		std::string name;
		Matrix4D    offset_transform;

		std::vector<CMeshBase*>	meshs;	//Associate meshs
		//Hirarcy...

		CObject*				parent;
		std::vector<CObject*>	children;
		Uint					generation;
		Uint					number;

		//more
		Uint					visible_mask;

		Uint object_id;


	public:
		CObject(String name_);
		CObject() : CObject("New_Object") {}

		CObject(CObject* source_);

		//Meshods...
		Void			SetName(String name_) { this->name = name_; }
		String			GetName() const { return name.c_str(); }
		
		Void			AddMesh(CMeshBase* mesh_) { this->meshs.push_back(mesh_); this->armature_link.bones_internal_offset.resize(meshs.size()); }
		CMeshBase*		GetMesh(UINT index_) const { return this->meshs.at(index_); }
		
		Uint			GetMeshsNumber() const { return meshs.size(); }
		
		Void			SetOffsetTransform(const Matrix4D &matrix_) { this->offset_transform = matrix_; }
		Matrix4D		GetOffsetTransform() const { return this->offset_transform; }
		Matrix4D*		GetTransformPointer(){return &offset_transform;}
		Matrix4D		GetTransform();
		Vector3D		GetPosition();
		//Hirarcy functions...
		CObject*	GetParent() const { return this->parent; }

		Void		CleanChildren() { this->children.clear(); }
		Uint		GetChildrenNumber() const { return this->children.size(); }
		Void		AddChild(CObject* new_child_);
		CObject*	GetChild(UINT index_) const { return (index_ >= 0 && index_ < children.size()) ? children.at(index_) : nullptr; }
		
		CObject* GetPreviousSibling() { return (number > 0) ? parent->GetChild(number - 1) : nullptr; }
		CObject* GetNextSibling() { return (number < parent->GetChildrenNumber()) ? parent->GetChild(number + 1) : nullptr; }

		CObject* GetFirstChild() { return (0 < children.size()) ? children.at(0) : nullptr; }
		CObject* GetLastChild() { return (0 < children.size()) ? children.at(children.size() - 1) : nullptr; }
		ArmatureLink GetArmatureLink() { return this->armature_link; }
		Void Replace(CObject* source_);

		Void LinkArmature(CObject* armature_) { armature_link.armature = armature_; }
		
		//virtual PE_Void SynchronizeWithGpu(PE_Uint frame_index_);
		//D3D12_GPU_VIRTUAL_ADDRESS GetGpuBufferAddres(PE_Uint frame_index_) { return object_buffers[frame_index_]->GetGpuBufferAddress(); }

		virtual Void Update() {};
		virtual Void Init(CGpuUploadTask* upload_list_) {};
		virtual Void Render(CGpuDrawTask* draw_task_, Uint layer_) {};

		Uint GetObjectId() { return object_id; }

		virtual ~CObject() {};
		
};
