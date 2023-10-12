#include "CArmature.h"

//Create an Armature Object from exist CObject
CArmature::CArmature(CObject* source_)
{
	CArmature* armature_cast = dynamic_cast<CArmature*>(source_);
	if (armature_cast != nullptr)
	{
		//Copy Armature data...
		this->bones_map			= armature_cast->bones_map;
		this->bones_poses		= armature_cast->bones_poses;
		this->bones_offset		= armature_cast->bones_offset;
		this->gpu_bones_poses	= armature_cast->gpu_bones_poses;
		this->gpu_bones_offsets = armature_cast->gpu_bones_offsets;

		//Relink influances objecs...
		std::set<CObject*> influances_;
		armature_cast->GetInfluanceObjects(&influances_);
		for (std::set<CObject*>::iterator it = influances_.begin(); it != influances_.end(); it++)
			(*it)->LinkArmature(this);
	}

	//Replace this object into the object hirarchy insted the source...
	Replace(source_);//Replace 
}

Void CArmature::GetInfluanceObjects(CObject* object_, std::set<CObject*>* influences_)
{
	const CBone* bone = dynamic_cast<const CBone*>(object_);
	if (bone != nullptr)
	{
		//Pass all involve object and they all mesh and is't all bones and insert into one big map...
		for (Uint i = 0; i < bone->influences.size() ; i++)
		{
			CObject* influance = bone->influences.at(i);
			if (influences_->find(influance) == influences_->end())
				influences_->insert(influance);
		}
	}

	//Continiue recursion into all children...
	for (Uint i = 0; i < object_->GetChildrenNumber(); i++)
		GetInfluanceObjects(object_->GetChild(i), influences_);
}

Void CArmature::GetInfluanceObjects(std::set<CObject*>* influences_)
{
	GetInfluanceObjects(this, influences_);
}

Void CArmature::CreateBonesTransforms()
{
	//Get all object that influance by this armature...
	std::set<CObject*> influances_;
	GetInfluanceObjects(&influances_);
	
	//Create map beteewn bone name and indecies pack of location in bone-transforms-buffer...
	
	bones_offset.push_back(Matrix4D(0.0f));//Fill first buffer element with zero matrix
	Uint ids_dispatch_offset = 0;
	for (std::set<CObject*>::iterator it = influances_.begin(); it != influances_.end(); it++)
	{
		(*it)->LinkArmature(this);//Link influances_ object to this armature

		for (Uint j = 0; j < (*it)->GetMeshsNumber(); j++)
		{
			(*it)->armature_link.bones_internal_offset.at(j) = ids_dispatch_offset + 1;

			for (Uint k = 0; k < (*it)->GetMesh(j)->GetBonesNumber() ; k ++)
			{
				std::string bone_name	= (*it)->GetMesh(j)->GetBoneName(k);
				Matrix4D bone_offset = (*it)->GetMesh(j)->GetBoneOffsetTransform(k);

				auto bone_iter = bones_map.find(bone_name);
				if (bone_iter == bones_map.end())
					bones_map.insert(std::pair<std::string, std::vector<Uint>>(bone_name, std::vector<Uint>({ ids_dispatch_offset + k + 1 })));
				else
					bone_iter->second.push_back(ids_dispatch_offset + k + 1);

				bones_offset.push_back(bone_offset);
			}

			ids_dispatch_offset += (*it)->GetMesh(j)->GetBonesNumber();
		}
	}

	bones_poses.resize(bones_offset.size());

	Engine::objects_manager->NewArmatureRigion(bones_offset.size());
}


Vector3D SampleTranslation(Double time_, Double duriation, const aiNodeAnim* channel_, InterpFunction inerpolation_func_)
{
	aiVector3D translation;

	if (channel_->mNumPositionKeys == 1)
	{
		translation = channel_->mPositionKeys[0].mValue;
	}
	else
	{
		Uint framekey_index = 0;

		Float add_duriation;
		if (time_ < channel_->mRotationKeys[0].mTime)
		{
			add_duriation = duriation;
			framekey_index = channel_->mNumRotationKeys - 1;
		}
		else
		{
			add_duriation = 0.0f;

			for (Uint i = 0; i < channel_->mNumRotationKeys - 1; i++)
			{
				if (time_ < channel_->mRotationKeys[i + 1].mTime)
				{
					framekey_index = i;
					break;
				}
			}
		}

		const aiVectorKey& current_framekey	= channel_->mPositionKeys[framekey_index];
		const aiVectorKey& next_framekey	= channel_->mPositionKeys[(framekey_index + 1) % channel_->mNumRotationKeys];
		Float delta = inerpolation_func_((time_ + add_duriation - (Float)current_framekey.mTime) / (Float)(next_framekey.mTime + add_duriation - current_framekey.mTime));
		
		const aiVector3D& start = current_framekey.mValue;
		const aiVector3D& end   = next_framekey.mValue;

		translation = current_framekey.mValue + delta * (next_framekey.mValue - current_framekey.mValue);
	}

	return *((Vector3D*)&translation);
}

Quaternion SampleRotation(Double time_, Double duriation, const aiNodeAnim* channel_, InterpFunction inerpolation_func_)
{
	aiQuaternion rotation;

	if (channel_->mNumRotationKeys == 1)
	{
		rotation = channel_->mRotationKeys[0].mValue;
	}
	else
	{
		Uint framekey_index = 0;

		Float add_duriation;
		if (time_ < channel_->mRotationKeys[0].mTime)
		{
			add_duriation = duriation;
			framekey_index = channel_->mNumRotationKeys - 1;
		}
		else
		{
			add_duriation = 0.0f;

			for (Uint i = 0; i < channel_->mNumRotationKeys - 1; i++)
			{
				if (time_ < channel_->mRotationKeys[i + 1].mTime)
				{
					framekey_index = i;
					break;
				}
			}
		}

		const aiQuatKey& current_framekey	= channel_->mRotationKeys[framekey_index];
		const aiQuatKey& next_framekey		= channel_->mRotationKeys[(framekey_index + 1) % channel_->mNumRotationKeys];
		Float delta = inerpolation_func_((time_ + add_duriation - (Float)current_framekey.mTime) / (Float)(next_framekey.mTime + add_duriation - current_framekey.mTime));
		

		aiQuaternion::Interpolate(rotation, current_framekey.mValue, next_framekey.mValue, delta);
		rotation.Normalize();
	}

	return *((Quaternion*)&rotation);
}

Vector3D SampleScale(Double time_, Double duriation, const aiNodeAnim* channel_, InterpFunction inerpolation_func_)
{
	aiVector3D scale;

	if (channel_->mNumScalingKeys == 1)
	{
		scale = channel_->mScalingKeys[0].mValue;
	}
	else
	{
		Uint framekey_index = 0;

		Float add_duriation;
		if (time_ < channel_->mRotationKeys[0].mTime)
		{
			add_duriation = duriation;
			framekey_index = channel_->mNumRotationKeys - 1;
		}
		else
		{
			add_duriation = 0.0f;

			for (Uint i = 0; i < channel_->mNumRotationKeys - 1; i++)
			{
				if (time_ < channel_->mRotationKeys[i + 1].mTime)
				{
					framekey_index = i;
					break;
				}
			}
		}

		const aiVectorKey& current_framekey	= channel_->mScalingKeys[framekey_index];
		const aiVectorKey& next_framekey	= channel_->mScalingKeys[(framekey_index + 1) % channel_->mNumRotationKeys];
		Float delta = inerpolation_func_((time_ + add_duriation - (Float)current_framekey.mTime) / (Float)(next_framekey.mTime + add_duriation - current_framekey.mTime));
		
		const aiVector3D& start = current_framekey.mValue;
		const aiVector3D& end   = next_framekey.mValue;

		scale = current_framekey.mValue + delta * (next_framekey.mValue - current_framekey.mValue);
	}

	return *((Vector3D*)&scale);
}

Void CArmature::UpdateBonesBuffers(Uint frame_index_)
{
	gpu_bones_poses[frame_index_]->Get_MappedMemory();
}

Void CArmature::ApplyAnimation(const CAnimation *animation_, Double time_, InterpFunction interpolation_func_)
{
	//Determine animation time...

	Float ticks_per_second	= (Float)(animation_->ticks_per_secound != 0 ? animation_->ticks_per_secound : 24.0f);
	Float time_in_ticks		= time_ * ticks_per_second;
	Float animation_time		= fmod(time_in_ticks, (Float)animation_->duriation);//The final animation time(in ticks)

	RecursiveUpdate(animation_, animation_time, interpolation_func_);
}


/*
PE_Void CArmature::ActiveDeflectedAnimation(CAnimation* animation_, PE_Double time_, CArmature::State state_, PE_Float deflect_factor_, InterpFunction interpolation_func_)
{
	if (state_.size() != bones_poses.size())
	{
		throw std::runtime_error("CArmature::ActiveDeflectedAnimation Error - The number of bones poses in the given armature state not apropriate to the this armature");
		return;
	}
	
	//Determine animation time...
	PE_Float ticks_per_second	= (PE_Float)(animation_->ticks_per_secound != 0 ? animation_->ticks_per_secound : 24.0f);
	PE_Float time_in_ticks		= time_ * ticks_per_second;
	PE_Float animation_time		= fmod(time_in_ticks, (PE_Float)animation_->duriation);//The final animation time(in ticks)

	RecursiveUpdate(animation_, animation_time, interpolation_func_);

	PE_Matrix4D* virtual_addres = (PE_Matrix4D*)gpu_bones_poses[Engine::graphics->GetCorrentFrameIndex()]->Map();

	for (PE_Uint i = 0; i < bones_poses.size(); i++)
	{
		bones_poses.at(i) = PE_Interpolate(bones_poses.at(i), state_.at(i), deflect_factor_);
		virtual_addres[i] = bones_poses.at(i);
	}

	//gpu_bones_poses[Engine::graphics->GetCorrentFrameIndex()]->Unmap();
}

PE_Void CArmature::ActiveBlendedAnimations(CAnimation* first_animation_, PE_Float first_time_, CAnimation* secound_animation_, PE_Float secound_time_, PE_Float blend_factor_, InterpFunction interpolation_func_)
{
	PE_Matrix4D* first_bones_poses = new PE_Matrix4D[bones_poses.size()];

	PE_Float tick_per_second, animation_time;
	
	tick_per_second = (first_animation_->ticks_per_secound != 0 ? first_animation_->ticks_per_secound : 24.0f);
	animation_time  = fmod(first_time_ * tick_per_second, (PE_Float)first_animation_->duriation);
	RecursiveUpdate(first_animation_, animation_time, interpolation_func_);
	
	memcpy(first_bones_poses, bones_poses.data(), sizeof(PE_Matrix4D) * bones_poses.size());

	tick_per_second = (secound_animation_->ticks_per_secound != 0 ? secound_animation_->ticks_per_secound : 24.0f);
	animation_time  = fmod(secound_time_ * tick_per_second, (PE_Float)secound_animation_->duriation);
	RecursiveUpdate(secound_animation_, animation_time, interpolation_func_);

	PE_Matrix4D* virtual_addres = (PE_Matrix4D*)gpu_bones_poses[Engine::graphics->GetCorrentFrameIndex()]->Map();

	for (PE_Uint i = 0; i < bones_poses.size(); i++)
	{
		bones_poses.at(i) = PE_Interpolate(first_bones_poses[i], bones_poses.at(i), blend_factor_);
		virtual_addres[i] = bones_poses.at(i);
	}

	gpu_bones_poses[Engine::graphics->GetCorrentFrameIndex()]->Unmap();
}
*/

Void CArmature::RecursiveUpdate(const CAnimation* animation_, Float animation_time_, InterpFunction interpolation_func_)
{
	for (Uint i = 0; i < children.size() ; i ++)
		RecursiveBonesUpdate(animation_, animation_time_, children.at(i), Matrix4D(1.0f), interpolation_func_);
}

Void CArmature::RecursiveBonesUpdate(const CAnimation* animation_, Float animation_time_, CObject* object_, Matrix4D perant_transform_, InterpFunction interpolation_func_)
{
	Matrix4D object_transform = object_->GetTransform();
	const aiNodeAnim* participator = nullptr;
	
	//Search for proper channel...
	auto iter = animation_->channels.find(object_->GetName());
	if (iter != animation_->channels.end())
		participator = iter->second;

	if (participator != nullptr)
	{
		// Get interpolated matrices between current and next frames
		Vector3D	  scale		  = SampleScale(animation_time_, animation_->duriation, participator, interpolation_func_);
		Quaternion	  rotation	  = SampleRotation(animation_time_, animation_->duriation, participator, interpolation_func_);
		Vector3D	  translation = SampleTranslation(animation_time_, animation_->duriation, participator, interpolation_func_);

		object_transform		 = perant_transform_ * Transform(translation, rotation, scale);
	}

	//Search for Bones channel...
	auto it = bones_map.find(object_->GetName());
	if (it != bones_map.end())
	{
		//Copy transform into bones transform buffer...
		for (Uint i = 0; i < it->second.size(); i++)
			bones_poses.at((it->second.at(i))) = object_transform;
	}

	for (UINT i = 0; i < object_->GetChildrenNumber() ; i ++)
		RecursiveBonesUpdate(animation_, animation_time_, object_->GetChild(i), object_transform, interpolation_func_);
}

Void CArmature::CaptureState(CArmature::State* state_)
{
	state_->resize(bones_poses.size());
	memcpy(state_->data(), bones_poses.data(), sizeof(Matrix4D) * bones_poses.size());
}