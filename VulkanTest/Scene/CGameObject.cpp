#include "CGameObject.h"


CObject::CObject(String name_) : object_id(Engine::objects_manager->NewObjectIdentefier())
{
	name					= name_;
	offset_transform		= Matrix4D(1);
	parent					= nullptr;
	generation				= 0;
	number					= 0;
	visible_mask			= 0xffffffff;//Visible in all draw layers
	armature_link.armature	= nullptr;
}

Void CObject::Replace(CObject* source_)
{
	//Copy information....
	name = source_->GetName();
	offset_transform = source_->GetOffsetTransform();

	for (UINT i = 0; i < source_->GetMeshsNumber(); i ++)
		AddMesh(source_->GetMesh(i));

	//source_->copy_animations(relevant_animations);

	//Replace target in hirarchy...

	parent			= source_->GetParent();
	generation		= source_->generation;
	number			= source_->number;
	object_id		= source_->object_id;
	source_->GetParent()->children.at(source_->number) = this;


	for (UINT i = 0; i < source_->GetChildrenNumber(); i ++)
	{
		AddChild(source_->GetChild(i));
		GetChild(i)->parent = this;
	}

	//Free the orginal...
	delete source_;
}

CObject::CObject(CObject* source_)
{
	Replace(source_);
}

Void CObject::AddChild(CObject* new_child_)
{
	if (new_child_ == nullptr)
		return;

	children.push_back(new_child_);
	new_child_->parent		= this;
	new_child_->generation  = generation + 1;
	new_child_->number		= children.size() - 1;
}

Matrix4D	CObject::GetTransform()
{
	Matrix4D transform = Matrix4D(1);

	CObject* temp = this;
	while (temp)
	{
		transform = temp->GetOffsetTransform() * transform;
		temp = temp->GetParent();
	}

	return transform;
}

Vector3D CObject::GetPosition()
{
	Matrix4D transform = GetTransform();
	return Vector3D(transform[0][3], transform[1][3], transform[2][3]);
}

/*
PE_Void CObject::SynchronizeWithGpu(PE_Uint frame_index_)
{
	//Update object's gpu buffer...
	Engine::GpuObjectStruct gpu_object_info;

	if(meshs.size() > 0)
		gpu_object_info.material_index = Engine::materials_menager->GetBufferLocation(meshs.at(0)->GetMaterial());
	else
		gpu_object_info.material_index = INFINITE;

	if (armature_link != nullptr && bone_ids_offsets.size() > 0)
	{
		gpu_object_info.mesh_transform   = armature_link->GetTransform();
		gpu_object_info.bones_ids_offset = bone_ids_offsets.at(0);
	}
	else
	{
		gpu_object_info.mesh_transform   = GetTransform();
		gpu_object_info.bones_ids_offset = INFINITE;
	}
	this;
	object_buffers[frame_index_]->Update(gpu_object_info);
}
*/